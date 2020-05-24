#include "Charset.h"
#include "Manager.h"

Charset::Charset(){
	my_object = (object*)RAM::malloc(sizeof(object));
}

void Charset::setFile(char* fileName){
	FileHandler::FILE newFile;
	if(Manager::fileHandler.getFile(fileName, &newFile)){	
		uint8_t* charset = RAM::malloc(newFile.size);
		Manager::fileHandler.fileToSpiRam(&newFile, charset, newFile.size);
		
		// CHR(3) + width(2) + height(2) + charWidth(1) + charHeight(1) + image(w*h/8)
		object temp;
		temp.width = RAM::read16(charset + 3);
		temp.height = RAM::read16(charset + 5);
		temp.charWidth = RAM::read(charset + 7);
		temp.charHeight = (RAM::read(charset + 8) + 1) * 8;
		temp.image = charset + 9;
		RAM::writeArray(my_object, &temp, sizeof(object));
	}
}

uint16_t Charset::getWidth(){
	return RAM::read16(&my_object->width);
}

uint16_t Charset::getHeight(){
	return RAM::read16(&my_object->height);
}

uint8_t Charset::getCharWidth(){
	return RAM::read(&my_object->charWidth);
}

uint8_t Charset::getCharHeight(){
	return RAM::read(&my_object->charHeight);
}

uint8_t* Charset::getImage(){
	return RAM::readPtr(&my_object->image);
}