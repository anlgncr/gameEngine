#ifndef SpriteSheet_h
#define SpriteSheet_h
#include "DisplayObject.h"
#include "FileHandler.h"
#include "Sprite.h"

class SpriteSheet : public DisplayObject{
	public:
		SpriteSheet();
		bool drawFromFile(char* fileName);
		Sprite* getSpriteByIndex(uint16_t index);
		Sprite* getSprite(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
		uint8_t* cropImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
	
	private:
};

#endif