#ifndef Charset_h
#define Charset_h

#include "Arduino.h"
#include "FileHandler.h"
#include "RAM.h"

class Charset{
	public:
		Charset();
	
		void setFile(char* fileName);
		uint16_t getWidth();
		uint16_t getHeight();
		uint8_t getCharWidth();
		uint8_t getCharHeight();
		uint8_t* getImage();
		
	private:
		struct object{
			uint8_t* image;
			uint16_t width;
			uint16_t height;
			uint8_t charWidth;
			uint8_t charHeight;
		};
		
		object* my_object;	
};
#endif