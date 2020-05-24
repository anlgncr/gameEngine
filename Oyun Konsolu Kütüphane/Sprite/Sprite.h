#ifndef Sprite_h
#define Sprite_h
#include "arduino.h"
#include "DisplayObject.h"
#include "RAM.h"
#include "Convert.h"

class Sprite : public DisplayObject{
	public:
		Sprite();
		Sprite(uint8_t childSize);

		void draw(uint8_t* image);
		void drawFromProgmem(uint8_t* image);
		bool drawFromFile(char* fileName);
		
		uint8_t update();

		
	private:
};

#endif