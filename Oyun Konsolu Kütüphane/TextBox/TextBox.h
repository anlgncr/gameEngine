#ifndef TextBox_h
#define TextBox_h
#include "arduino.h"
#include "DisplayObject.h"
#include "RAM.h"
#include "MovieClip.h"
#include "Charset.h"
//#include "ASCII.h"

#define TYPE_8 1
#define TYPE_16 2
#define TYPE_24 3
#define TYPE_32 4

const uint8_t cursor[] PROGMEM = {1,0,8,0, 0xFF};

class TextBox : public DisplayObject{
	
	public:
		TextBox();
		TextBox(uint8_t childSize);
		bool create(uint16_t width, uint16_t rowCount);
		bool create(uint16_t width, uint16_t rowCount, uint8_t type);
		
		void write(char* text);
		void writeFromProgmem(char* text);
		bool writeFromFile(char* text);
		void clear();
		
		uint8_t update();
		void setCursor(uint8_t);
	
	private:
		struct object{
			Charset* charset;
			uint8_t *textIndex;
			uint8_t *imageIndex;
		};
		object *my_object;
		
		bool createTextBox(uint16_t width, uint16_t rowCount, uint8_t type);
		void writeToTextBox(char* text, uint8_t textSource);
		
		void decodeUTF8(uint8_t* text, uint8_t* UTF8_encodedText, uint16_t length);
		uint16_t getUTF8CharCount(uint8_t* text, uint16_t length);
		
		MovieClip cursor_clip;
		uint8_t *cursor_images[2] = {(uint8_t*)cursor, NULL};
		void moveCursor(uint8_t, uint8_t);
		
		void setCharset(uint8_t type);
		void setTextIndex(uint16_t);
		void setImageIndex(uint8_t*);
		void setCharset(Charset*);
		
		Charset* getCharset();
		uint16_t getTextIndex();
		uint8_t* getImageIndex();
		uint8_t getTextSource();
};

#endif