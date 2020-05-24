#include "Manager.h"

Manager manager;
TextBox myText;
const char yazi[] PROGMEM = "Merhaba! Bu yazi Atmega328' in program hafizasinda saklaniyor";

void setup(){
	manager.begin();	

	myText.create(128, 8);
	myText.setCursor(true);
	myText.writeFromProgmem((char*)yazi);
	Manager::scene.addChild(&myText);
}

void loop(){
  manager.run();
}