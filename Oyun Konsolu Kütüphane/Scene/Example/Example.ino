#include "setup.h"

 
 Shape my_shape(1);
 Sprite my_sprite(1);
 MovieClip my_clip(1, NULL, 0, 0);
 
void setup() {
 Serial.begin(9600);
 
 my_scene.setX(100);
 my_scene.setY(200);

 my_scene.setName("Sahne");
 my_shape.setName("Shape");
 my_sprite.setName("Sprite");
 my_clip.setName("Movie");
 
 my_scene.addChild((Drawable*)&my_sprite);
 my_scene.addChild((Drawable*)&my_shape);
 my_scene.addChild((Drawable*)&my_clip);

 my_scene.update();
 my_scene.writeInfo();
}

void loop() {

}