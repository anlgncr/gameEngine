#include "Manager.h"

Manager manager;
Sprite container(32); 

void changeValues(Tween::tweenObject* t){
   Tween::tweenObject* tween = Tween::start(container.getXRef(), -5, 5, 2000, SPIMEM);
   Tween::setRepeatCount(tween, 2);
   Tween::setReverse(tween, true);
   Tween::setOnFinish(tween, changeValues);
   Tween::setAnimation(tween, random(1,3));
}

void setup() {
	manager.begin();

	Manager::scene.addChild(&container);
	
	MovieClip source;
	MovieClip *boxes;
	source.createFromFile("boxes.m");

  for(byte j=0; j<4; j++){
  	for(byte i=0; i<8; i++){
  		boxes = new MovieClip();
      Serial.println((uint16_t)boxes);
  		boxes->createInstanceOf(&source);
  		boxes->setXY(16 * i, 16 * j);
  		boxes->play();
  		boxes->setFPS(random(16,24));
  		container.addChild(boxes);
  	}
  }
	
	/*Tween::tweenObject* tween = Tween::start(container.getXRef(), -5, 5, 2000, SPIMEM);
	Tween::setRepeatCount(tween, 2);
	Tween::setReverse(tween, true);
	Tween::setOnFinish(tween, changeValues);*/
}

void loop() {
	manager.run();
}