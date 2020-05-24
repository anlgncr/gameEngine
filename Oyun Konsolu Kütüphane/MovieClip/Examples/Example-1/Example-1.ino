#include "Manager.h"

Manager manager;
MovieClip pikachu;

void setup() {
	manager.begin();
	pikachu.createFromFile("pika.mov");
	pikachu.play();
	Manager::scene.addChild(&pikachu);
}

void loop() {
	manager.run();
}