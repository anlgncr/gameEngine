#ifndef Shape_h
#define Shape_h
#include "arduino.h"
#include "DisplayObject.h"
#include "RAM.h"

class Shape : public DisplayObject{
	public:
		Shape(uint8_t childSize) : DisplayObject(childSize){};
		Shape() : DisplayObject(0){};
		
		void create(uint16_t width, uint16_t height){
			if(width == 0 || height == 0)
				return;
			
			uint16_t rowCount = height / 8;
			rowCount += ((height % 8) == 0) ? 0 : 1;
	
			uint8_t *image = (uint8_t*)RAM::malloc(width * rowCount + 4);
			if(!image) { return; }
				
			RAM::write16(&image[0], width);
			RAM::write16(&image[2], rowCount * 8);
			
			setMemory(SPIMEM);
			setImage(image);
			setHeight(height);
			setWidth(width);
		}
		
		void putPixel(int16_t x, int16_t y){		
			uint16_t width = getWidth();
			if(x >= width || x < 0)
				return;
			
			uint16_t height = getHeight();
			if(y >= height || y < 0)
				return;
			
			uint8_t currentRow = y / 8;
			uint8_t shift = y % 8;
			
			uint8_t *image = &(getImage()[4]);
			uint8_t(*image_array)[width];
			image_array = (uint8_t(*)[width])(image);
	
			uint8_t data = RAM::read(&image_array[currentRow][x]);
			RAM::write(&image_array[currentRow][x], 1 << shift | data);
		};
		
		void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
			if(!getImage())
				return;
			
			int16_t disX = x2 - x1;
			int16_t disY = y2 - y1;
			
			float hypotenuse = sqrt((disX * disX) + (disY * disY));
			
			float verticalMove = (float)disY / (float)hypotenuse;
			float horizontalMove = (float)disX / (float)hypotenuse;
			
			float curX = x1, curY = y1;
			for(uint16_t i=0; i<=(uint16_t)hypotenuse; i++){
				putPixel(round(curX), round(curY));
				curX += horizontalMove;
				curY += verticalMove;
			}
		};
		
		void drawCircle(int16_t x, int16_t y, int16_t radius){
			if(!getImage())
				return;
			
			for(float i=0; i<360; i++){
				float radian = (i / 180) * PI;
				int16_t x1 = round(sin(radian) * radius) + x;
				int16_t y1 = round(cos(radian) * radius) + y;
				putPixel(x1, y1);
			}
		};
		
		void drawRectangle(int16_t x, int16_t y, uint16_t rect_width, uint16_t rect_height){
			if(!getImage())
				return;
			
			uint16_t width = getWidth();
			uint16_t height = getHeight();
			
			if(x >= width || y >= height || x + width <= 0 || y + height <= 0)
				return;
			
			for(uint16_t i=0; i<rect_width; i++){
				putPixel(x + i, y);	
			}
			
			for(uint16_t i=0; i<rect_width; i++){
				putPixel(x + i, y + rect_height - 1);
			}
			
			for(uint16_t i=0; i<rect_height; i++){
				putPixel(x, y + i);
			}
		
			for(uint16_t i=0; i<rect_height; i++){
				putPixel(x + (rect_width - 1), y + i);
			}	

		};
		
		void drawSquare(int16_t x, uint16_t y, uint16_t width){
			drawRectangle(x, y, width, width);
		};
		
		void fill(){
			uint16_t width = getWidth();
			uint16_t height = getHeight() / 8;
			uint8_t shift = getHeight() % 8;
			
			uint8_t *image = &getImage()[4];
			uint8_t(*image_array)[width];
			image_array = (uint8_t(*)[width])(image);
			
			for(uint8_t currentRow = 0; currentRow < height; currentRow++){
				for(uint8_t x=0; x<width; x++){
					if(currentRow == (height - 1)){
						RAM::write(&image_array[currentRow][x], 0xFF >> (8 - shift));
					}
					else{
						RAM::write(&image_array[currentRow][x], 0xFF);
					}
				}
			}
		}
		
		void clear(){
			uint16_t width = getWidth();
			uint16_t height = getHeight() / 8;
			
			RAM::writeLoop(&getImage()[4], 0x00, width * height);
		}
		
		uint8_t update(){
			
		};
				
	private:
};

#endif