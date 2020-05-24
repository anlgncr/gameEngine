#ifndef Buttons_h
#define Buttons_h
#include "arduino.h"
#include <SPI.h>
#include <RAM.h>

#define DEBOUNCE_TIME 40

#define BUTTON_H	10
#define BUTTON_G	9
#define BUTTON_F	8
#define BUTTON_E	7
#define BUTTON_D	6
#define BUTTON_C	5
#define BUTTON_B	4
#define BUTTON_A	3
#define BUTTON_Y	2
#define BUTTON_X	1

#define CODE_H	128	
#define CODE_G	64 
#define CODE_F	32 
#define CODE_E	16 
#define CODE_D	8  
#define CODE_C	4  
#define CODE_B	2  
#define CODE_A	1  
#define CODE_Y	2  
#define CODE_X	1  

#define SPI_INPUT 0
#define PARALEL_INPUT 1

class Button{
  public:
	typedef void (*fPtr)(Button*);
	Button(uint8_t, fPtr);
		
	void setInputType(uint8_t);
	uint8_t getInputType();
	void update(uint8_t);
	bool buttonUp();
	bool buttonDown();
	bool buttonLongDown();

	uint8_t getCode();
	uint8_t getPin();
	uint8_t isDown();
	
  private:
	struct object{
		uint8_t pin;
		bool up;
		bool down;
		bool pushed;
		bool released;
		bool longPressed;
		bool preLongPressed;
		unsigned long up_lastTime;
		unsigned long down_lastTime;
		unsigned long longPressedTime;
		fPtr onChanged;
		uint8_t inputType;
	};
	object *my_object;
  
	void statusChanged();
	void copyObject(object*, object*);
	void loadObject(object*, object*);
	
};

#endif