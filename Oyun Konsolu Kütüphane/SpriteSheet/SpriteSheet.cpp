#include "SpriteSheet.h"
#include "Manager.h"

SpriteSheet::SpriteSheet() : DisplayObject(0){ }

bool SpriteSheet::drawFromFile(char* fileName){
	FileHandler::FILE newFile;
	bool fileExists = Manager::fileHandler.getFile(fileName, &newFile);
	if(fileExists && newFile.size > 0)
	{		
		uint8_t data[7];
		Manager::fileHandler.fileToSram(&newFile, data, 7);

		uint16_t width = Convert::toUInt16(&data[3]);
		uint16_t height = Convert::toUInt16(&data[5]);
	
		if(height != 0 && width != 0){
			uint8_t* image = RAM::malloc(newFile.size);
			Manager::fileHandler.fileToSpiRam(&newFile, image, newFile.size);
			setMemory(SPIMEM);
			setImage(image + 3);
			return true;
		}
	}
	return false;
}


Sprite* SpriteSheet::getSprite(uint16_t x, uint16_t y, uint16_t width, uint16_t height){
	Sprite* new_sprite = new Sprite;
	
	if(uint8_t* croppedImage = cropImage(x, y, width, height)){
		new_sprite->setMemory(SPIMEM);
		new_sprite->setImage(croppedImage);
		return new_sprite;	
	}

	new_sprite->setMemory(PGMEM);
	new_sprite->setImage(noImage);
	return new_sprite;	
}

Sprite* SpriteSheet::getSpriteByIndex(uint16_t index){
	Sprite* new_sprite = new Sprite;
	new_sprite->setMemory(PGMEM);
	new_sprite->setImage(noImage);
	
	uint8_t* paramPtr = getImage() + 4 + (getCanvasWidth() * (getCanvasHeight() / 8));
	uint16_t paramCount = RAM::read16(paramPtr); // total image count
	
	if(index >= paramCount)
		return new_sprite;
	
	//(10)name, (2)x, (2)y, (2)width, (2)height, (1)mask
	paramPtr += index*19 + 12; //(2)Sprite count + (10)name length
	
	uint16_t x = RAM::read16(paramPtr);
	uint16_t y = RAM::read16(paramPtr + 2);
	uint16_t width = RAM::read16(paramPtr + 4);
	uint16_t height = RAM::read16(paramPtr + 6);
	uint8_t mask = RAM::read16(paramPtr + 8);
	
	if(uint8_t* croppedImage = cropImage(x, y, width, height)){
		new_sprite->setMemory(SPIMEM);
		new_sprite->setImage(croppedImage);
		new_sprite->setMaskType(mask);

		if(mask == IMAGE_SELF_MASK){
			RAM::write16(croppedImage + 2, height / 2);
		}
	}
	return new_sprite;
}

uint8_t* SpriteSheet::cropImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height){
	if(getImage() && width != 0 && height != 0)
	{
		
		uint16_t sheetWidth = getCanvasWidth();
		uint16_t sheetHeight = getCanvasHeight();
		
		if(y >= sheetHeight || x >= sheetWidth)
			return;
		
		if((x + width) > sheetWidth){
			width = sheetWidth - x;
		}
		if((y + height) > sheetHeight){
			height = sheetHeight - y;
		}
			
		if(!((y + height) > sheetHeight))
		{				
			uint16_t partRowCount = height / 8;
			partRowCount += (height % 8 == 0)? 0 : 1;
			
			uint16_t sheetStartRow = y / 8;
			uint8_t imageShift = y % 8;
			uint8_t bottomSpace = (8 - (height % 8)) % 8;
			uint16_t sheetEndRow = (y + height - 1) / 8;
			height = partRowCount * 8;
			
			uint8_t* croppedImage = RAM::malloc(width * partRowCount + 4);
			RAM::writeArray(croppedImage, &width, 2);
			RAM::writeArray(croppedImage + 2, &height, 2);
			
			uint8_t (*imageArray)[width] = (uint8_t(*)[width])(croppedImage + 4);
			uint8_t (*sheetImage)[sheetWidth] = (uint8_t(*)[sheetWidth])(&getImage()[4]);
			
			for(uint16_t currentRow = 0; currentRow < partRowCount; currentRow++)
			{
				for(uint16_t currentCol = 0; currentCol < width; currentCol++) //Ust satır
				{
					uint8_t data = RAM::read(&sheetImage[sheetStartRow + currentRow][x + currentCol]);
					RAM::write(&imageArray[currentRow][currentCol], (data >> imageShift));	
				}
				
				if(imageShift && (currentRow + sheetStartRow < sheetEndRow)) //Alt satır
				{
					for(uint16_t currentCol = 0; currentCol < width; currentCol++)
					{
						uint8_t dataPart = RAM::read(&imageArray[currentRow][currentCol]);
						uint8_t dataSheet = RAM::read(&sheetImage[sheetStartRow + currentRow + 1][x + currentCol]);
						RAM::write(&imageArray[currentRow][currentCol], (dataSheet << (8 - imageShift)) | dataPart);
					}
				}
				//Maskeleme işlemi
				if(currentRow == (partRowCount - 1) && bottomSpace != 0){ // Son satır
					for(uint16_t currentCol = 0; currentCol < width; currentCol++)
					{
						uint8_t data = RAM::read(&imageArray[currentRow][currentCol]);
						uint8_t mask = 0xFF >> bottomSpace;
						RAM::write(&imageArray[currentRow][currentCol], data & mask);
					}
				}
			}
			return croppedImage;
		}
	}
	return NULL;
}