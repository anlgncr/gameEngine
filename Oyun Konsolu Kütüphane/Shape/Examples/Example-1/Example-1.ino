#include "Manager.h"
Manager manager;

Shape myShape;
void setup() {
  manager.begin();
  myShape.create(128, 64);
  myShape.drawRectangle(0, 0, 128, 64);
  myShape.drawCircle(64, 32, 20);
  myShape.drawLine(0, 0, 128, 64);
  myShape.drawLine(128, 0, 0, 64);
  
  Manager::scene.addChild(&myShape);
}

void loop() {
  manager.run();
}