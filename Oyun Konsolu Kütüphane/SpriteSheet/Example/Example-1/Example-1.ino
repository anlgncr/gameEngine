#include "Manager.h"
#include "SpriteSheet.h"

Manager manager;
SpriteSheet sheet;

void setup() {
	manager.begin();
  Sprite a;
  a.drawFromFile("image.bmp");
  Manager::scene.addChild(&a);

  Sprite b;
  b.drawFromFile("a.b");
  Manager::scene.addChild(&b);
  b.setX(50);
  
  
	if(sheet.drawFromFile("pika.s")){
	  Sprite *s = sheet.getSpriteByIndex(0);  
	  Manager::scene.addChild(s);
	}
}

void loop() {
	manager.run();
}