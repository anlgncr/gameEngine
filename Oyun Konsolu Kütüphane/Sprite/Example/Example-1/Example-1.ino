#include "Manager.h"

Manager manager;
Sprite image;

void setup() {
	manager.begin();
	image.drawFromFile("image.bmp");

	Manager::scene.addChild(&image);
}

void loop() {
	manager.run();
}