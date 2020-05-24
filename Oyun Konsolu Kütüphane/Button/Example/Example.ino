#include <Manager.h>
Manager manager;
const uint8_t button_down[] PROGMEM = { 10, 2,
	0x78, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0x78, 
	0x00, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x00
};
const uint8_t button_up[] PROGMEM = { 10, 2, 
	0x78, 0x86, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x86, 0x78, 
	0x00, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x00
};
const uint8_t button_long_down[] PROGMEM = { 10, 2, 
	0x00, 0x78, 0xFC, 0xFE, 0xFE, 0xFE, 0xFE, 0xFC, 0x78, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00
};

const char header[] PROGMEM = "Input test...";

uint8_t* movie_button[3] = {(uint8_t*)button_up, (uint8_t*)button_down, (uint8_t*)button_long_down};

Sprite container(10);
TextBox myText(1, 128, 1, TYPE_8);

MovieClip movieA(0, movie_button, 3, 1);
MovieClip movieB(0, movie_button, 3, 1);
MovieClip movieC(0, movie_button, 3, 1);
MovieClip movieD(0, movie_button, 3, 1);
MovieClip movieE(0, movie_button, 3, 1);
MovieClip movieF(0, movie_button, 3, 1);
MovieClip movieG(0, movie_button, 3, 1);
MovieClip movieH(0, movie_button, 3, 1);
MovieClip movieX(0, movie_button, 3, 1);
MovieClip movieY(0, movie_button, 3, 1);

void setup() {
 
  myText.setTextSource(PGMEM);
  myText.write((char*)header);
  
  pinMode(7, OUTPUT);
  
  Manager::scene.addChild(&container);
  Manager::scene.addChild(&myText);
  
  container.addEventListener(BUTTON_DOWN, buttonDownListener);
  container.addEventListener(BUTTON_UP, buttonUpListener);
  container.addEventListener(BUTTON_LONG_DOWN, buttonLongDownListener);
  
  container.addChild(&movieA);
  container.addChild(&movieB);
  container.addChild(&movieC);
  container.addChild(&movieD);
  container.addChild(&movieE);
  container.addChild(&movieF);
  container.addChild(&movieG);
  container.addChild(&movieH);
  container.addChild(&movieX);
  container.addChild(&movieY);
  
  movieA.setXY(60, 20);
  movieB.setXY(70, 10);
  movieC.setXY(60, 0);
  movieD.setXY(50, 10);
  movieE.setXY(10,20);
  movieF.setXY(20,10);
  movieG.setXY(10,0);
  movieH.setXY(0,10);
  movieY.setXY(20, 30);
  movieX.setXY(70, -10);
  
  container.setXY(24, 17);
}

void buttonDownListener(DisplayObject *e){
	switch(e->getEventInfo()){
		case CODE_A: movieA.gotoAndStop(1); break;
		case CODE_B: movieB.gotoAndStop(1); break;
		case CODE_C: movieC.gotoAndStop(1); break;
		case CODE_D: movieD.gotoAndStop(1); break;
		case CODE_E: movieE.gotoAndStop(1); break;
		case CODE_F: movieF.gotoAndStop(1); break;
		case CODE_G: movieG.gotoAndStop(1); break;
		case CODE_H: movieH.gotoAndStop(1); break;
		case CODE_X: movieX.gotoAndStop(1); break;
		case CODE_Y: movieY.gotoAndStop(1); break;
	}
}

void buttonLongDownListener(DisplayObject *e){
	switch(e->getEventInfo()){
		case CODE_A: movieA.gotoAndStop(2); break;
		case CODE_B: movieB.gotoAndStop(2); break;
		case CODE_C: movieC.gotoAndStop(2); break;
		case CODE_D: movieD.gotoAndStop(2); break;
		case CODE_E: movieE.gotoAndStop(2); break;
		case CODE_F: movieF.gotoAndStop(2); break;
		case CODE_G: movieG.gotoAndStop(2); break;
		case CODE_H: movieH.gotoAndStop(2); break;
		case CODE_X: movieX.gotoAndStop(2); break;
		case CODE_Y: movieY.gotoAndStop(2); break;
	}
	digitalWrite(7, HIGH);
}

void buttonUpListener(DisplayObject *e){
	switch(e->getEventInfo()){
		case CODE_A: movieA.gotoAndStop(0); break;
		case CODE_B: movieB.gotoAndStop(0); break;
		case CODE_C: movieC.gotoAndStop(0); break;
		case CODE_D: movieD.gotoAndStop(0); break;
		case CODE_E: movieE.gotoAndStop(0); break;
		case CODE_F: movieF.gotoAndStop(0); break;
		case CODE_G: movieG.gotoAndStop(0); break;
		case CODE_H: movieH.gotoAndStop(0); break;
		case CODE_X: movieX.gotoAndStop(0); break;
		case CODE_Y: movieY.gotoAndStop(0); break;
	}
	digitalWrite(7, LOW);
}

void loop() {
  manager.run();
}