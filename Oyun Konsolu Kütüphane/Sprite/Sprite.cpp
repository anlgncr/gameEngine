#include "Sprite.h"
#include "Manager.h"

Sprite::Sprite() : DisplayObject(0){ }
Sprite::Sprite(uint8_t childSize) : DisplayObject(childSize){ }

bool Sprite::drawFromFile(char* fileName){
	FileHandler::FILE newFile;
	bool fileExists = Manager::fileHandler.getFile(fileName, &newFile);
	if(fileExists && newFile.size > 0)
	{		
		uint8_t data[8];
		Manager::fileHandler.fileToSram(&newFile, data, 8);

		uint16_t width = Convert::toUInt16(&data[4]);
		uint16_t height = Convert::toUInt16(&data[6]);
	
		if(height != 0 && width != 0){
			uint8_t* image = RAM::malloc(newFile.size);
			Manager::fileHandler.fileToSpiRam(&newFile, image, newFile.size);
			uint8_t maskTye = data[3];
			
			setMaskType(maskTye);
			setImage(image + 4);
			setMemory(SPIMEM);
			
			if(maskTye == IMAGE_SELF_MASK){
				RAM::write16(image + 6, height / 2);
			}
			
			return true;
		}
	}
	setMemory(PGMEM);
	setImage((uint8_t*)noImage);
	return false;
}

void Sprite::drawFromProgmem(uint8_t* image){
	setImage(image);
	setMemory(PGMEM);
}

void Sprite::draw(uint8_t* image){
	setImage(image);
	setMemory(SMEM);
}

uint8_t Sprite::update(){
	
}

