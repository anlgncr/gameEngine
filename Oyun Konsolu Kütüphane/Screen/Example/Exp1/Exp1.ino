#include <Scene.h>
#include <Sprite.h>
#include "image.h"
#include <Button.h>
#include <MovieClip.h>
#include <Updater.h>
#include <Tween.h>
#include <Delay.h>
#include <TextBox.h>

#define LEFT_ARROW    1
#define UP_ARROW      0
#define RIGHT_ARROW   3
#define DOWN_ARROW    2

#define LEFT_ARROW2    5
#define UP_ARROW2      4
#define RIGHT_ARROW2   7
#define DOWN_ARROW2    6

Updater engine(100);

uint8_t *images[8] = {(uint8_t*)img1,(uint8_t*)img2,(uint8_t*)img3,(uint8_t*)img4,
                      (uint8_t*)img5,(uint8_t*)img6,(uint8_t*)img7,(uint8_t*)img8};

uint8_t *dots[8] = {(uint8_t*)dot1,(uint8_t*)dot2,(uint8_t*)dot3,(uint8_t*)dot4,
                      (uint8_t*)dot5,(uint8_t*)dot6,(uint8_t*)dot7,(uint8_t*)dot8};

const char yazi[] PROGMEM = "Hello World!";


TextBox myText(1, 72, 1, TYPE_8);
Delay my_delay;
Tween my_tween(SPIMEM);
Scene myScene(200);
Sprite container(100);
Sprite container2(100);
Sprite sprite_resim(1);
Sprite sprite_resim2(1);
MovieClip my_clip(1, images, 8, 12);

void buttonListener(Button* my_button)
{ 
	if(my_button->buttonUp()){
		switch(my_button->getPin()){
		  case LEFT_ARROW2   :  myScene.removeChild(&container); break;
		  case UP_ARROW2     :  myScene.addChild(&container2); break;
		  case RIGHT_ARROW2  :  myScene.addChild(&container); break;
		  case DOWN_ARROW2   : myScene.removeChild(&container2);break;
		}
	}
}

Button left(LEFT_ARROW, buttonListener);
Button right(RIGHT_ARROW, buttonListener);
Button up(UP_ARROW, buttonListener);
Button down(DOWN_ARROW, buttonListener);
Button left2(LEFT_ARROW2, buttonListener);
Button right2(RIGHT_ARROW2, buttonListener);
Button up2(UP_ARROW2, buttonListener);
Button down2(DOWN_ARROW2, buttonListener);


void tweenListener(Tween* my_tween){
	Serial.println("tween listener");
}

void delayListener(Delay* my_delay){
	Serial.println("delay listener");
}

int16_t my_var;

unsigned long fps;
unsigned long time1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  
 myScene.setName("Sahne");
 myScene.addChild(&container);
 myScene.addChild(&container2);
 myScene.addChild(&sprite_resim);
 //myScene.addChild(&sprite_resim2);
 //sprite_resim2.setX(128);
 //sprite_resim2.setImage((uint8_t*)resim2);
 
 sprite_resim.setX(128);
 sprite_resim.setImage((uint8_t*)resim);
 
 myScene.addChild(&myText);
 myText.setTextSource(PGMEM);
 myText.write((char*)yazi);

/*uint8_t id = 1;
for(int i=0;i<4; i++){
	for(int j=0;j<8;j++){
		MovieClip *m = new MovieClip(1, images, 8, random(6, 18));
		container2.addChild(m);
		char name[3];
		sprintf(name,"%d", id++);
		m->setName(name);
		m->setX(j*16);
		m->setY(i*16);
		m->gotoAndPlay(random(7));
	}
}

container.setY(64);
for(int i=0;i<4; i++){
	for(int j=0;j<8;j++){
		MovieClip *m = new MovieClip(1, dots, 8, random(6, 18));
		container.addChild(m);
		char name[3];
		sprintf(name,"%d", id++);
		m->setName(name);
		m->setX(j*16);
		m->setY(i*16);
		m->gotoAndPlay(random(7));
	}
}*/

 engine.add(&myScene);
 engine.add(&left);
 engine.add(&right);
 engine.add(&up);
 engine.add(&down);
 engine.add(&left2);
 engine.add(&right2);
 engine.add(&up2);
 engine.add(&down2);

 
 /*my_tween.setOnRepeat(tweenListener);
 my_tween.setReverse(true);
 my_tween.setAnimation(EASE_OUT_BOUNCE);
 my_tween.setRepeatCount(0);
 my_tween.start(myScene.getXRef(), 0, 56, 1000);
 engine.add(&my_tween);*/

 /*
 my_delay.start(delayListener, 1000);
 engine.add(&my_delay);*/
}


void loop() {
  // put your main code here, to run repeatedly:
 fps = millis()-time1;
 time1 = millis();
 //Serial.println(my_var);
 
 engine.update();
 
 if(up.isDown())
	 myScene.setY(myScene.getY()-1);
 if(down.isDown())
	 myScene.setY(myScene.getY()+1); 
 if(left.isDown())
	 myScene.setX(myScene.getX()-1);
 if(right.isDown())
	  myScene.setX(myScene.getX()+1);
}