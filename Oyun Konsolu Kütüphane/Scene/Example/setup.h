#include <RAM.h>
#include <Drawable.h>
#include <Shape.h>
#include <Sprite.h>
#include <MovieClip.h>
#include <Scene.h>
#include <Button.h>

#define LEFT_ARROW    1
#define UP_ARROW      0
#define RIGHT_ARROW   3
#define DOWN_ARROW    2

#define BUTTON_DOWN 0
#define BUTTON_UP 1
#define BUTTON_LONG_DOWN 2

Scene my_scene(100);

void buttonListener(Button* my_button)
{ 
	uint8_t button_event;
	
	button_pin = my_button->getPin();
	
	if(my_button->buttonUp()){
		button_event = BUTTON_UP;
	}
	else if(my_button->buttonDown()){
		button_event = BUTTON_DOWN;
	}
	else if(my_button->buttonLongDown()){
		button_event = BUTTON_LONG_DOWN;
	}
	else{
		return;
	}
};