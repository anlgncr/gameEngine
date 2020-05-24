#include "Manager.h"

Scene Manager::scene(100);
FileHandler Manager::fileHandler;
Charset Manager::charset8;
Charset Manager::charset16;
uint32_t Manager::processTime = 0;

uint32_t Manager::getProcessTime(){
	return processTime;
}

uint8_t Manager::getBrightness(){
	uint8_t brightness = map(analogRead(A2), 0, 1023, 0, 255);
	return (255 - brightness);
}

void Manager::buttonListener(Button* my_button){ 
	uint8_t button_event;
	if(my_button->buttonUp())
		button_event = BUTTON_UP;
	else if(my_button->buttonDown())
		button_event = BUTTON_DOWN;
	else if(my_button->buttonLongDown())
		button_event = BUTTON_LONG_DOWN;
	else
		return;
	
	EventArgs e;
	e.setSender(&scene);
	e.setEventName(button_event);
	e.setData(my_button->getCode());
	scene.dispatchEventAll(&e);
}

Manager::Manager():
	button_A(CODE_A, buttonListener), button_B(CODE_B, buttonListener),
	button_C(CODE_C, buttonListener), button_D(CODE_D, buttonListener),
	button_E(CODE_E, buttonListener), button_F(CODE_F, buttonListener),
	button_G(CODE_G, buttonListener), button_H(CODE_H, buttonListener),
	button_X(CODE_X, buttonListener), button_Y(CODE_Y, buttonListener)
{		  
	SPI.begin();
	
	pinMode(LIGHT_SENSOR, INPUT);
	pinMode(CHARGE_STATUS_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(LED, OUTPUT);
	
	spiSetting = SPISettings(16000000, MSBFIRST, SPI_MODE0);
	
	button_X.setInputType(PARALEL_INPUT);
	button_Y.setInputType(PARALEL_INPUT);
}

void Manager::begin(){
	Serial.begin(115200);
	Tween::begin();
	Delay::begin();
	
	charset8.setFile("c8.c");
	charset16.setFile("c16.c");
	
	updateButtons();
	proccessing.createFromFile("circles.m");
	proccessing.setXY(112, 48);
}

void Manager::showLoading(bool show){
	if(show == loading)
		return;
	
	if(show){
		scene.addChild(&proccessing);
		proccessing.play();
	}
	else{
		scene.removeChild(&proccessing);
		proccessing.gotoAndStop(0);
	}
	
	loading = show;
}

void Manager::run(){
	if(Serial.available()){
		readStartTime = millis();
	}
	
	while(Serial.available()){
		serialHandler.processByte(Serial.read());
	}
	
	if(millis() > readStartTime + READ_TIMEOUT){
		showLoading(false);
	}
	else{
		showLoading(true);
	}
	
	static uint32_t lastMillis = 0;
	
	updateButtons();
	Tween::update();
	Delay::update();
	
	scene.onUpdate();
	scene.applyChildChanges();
	for(uint8_t i=0; i<scene.getChildCount(); i++){
		updateDisplayObjects(scene.getChildAt(i));
	}
	processTime = millis() - lastMillis;
	lastMillis = millis();
}

void Manager::updateDisplayObjects(DisplayObject* child){
	if(child == NULL)
		return;

	child->setGlobalX(child->getParent()->getGlobalX() + child->getX());
	child->setGlobalY(child->getParent()->getGlobalY() + child->getY());
	child->onUpdate();
				
	if(child->isDelaying()){
		if(millis() > (child->getDelayStartTime() + child->getDelayTime())){
			child->onDelay();
			child->setDelayStartTime(millis());	
		}
	}
	
	if(child->getChildCount() > 0){
		child->applyChildChanges();
		for(uint8_t i=0; i<child->getChildCount(); i++){					
			updateDisplayObjects(child->getChildAt(i));
		}
	}
}

void Manager::updateButtons(){	
	uint8_t data;
	
	SPI.beginTransaction(spiSetting);
	PORTC &= DECODER_MASK;
	PORTC |= CS_BUTTON;
	data = SPI.transfer(0);
	SPI.endTransaction();
	
	button_A.update(data);
	button_B.update(data);
	button_C.update(data);
	button_D.update(data);
	button_E.update(data);
	button_F.update(data);
	button_G.update(data);
	button_H.update(data);
	
	data = PINB & PARALEL_BUTTON_MASK;
	button_X.update(data);
	button_Y.update(data);
}

uint8_t Manager::isButtonDown(uint8_t code){
	switch(code){
		case BUTTON_A : return button_A.isDown();
		case BUTTON_B : return button_B.isDown();
		case BUTTON_C : return button_C.isDown();
		case BUTTON_D : return button_D.isDown();
		case BUTTON_E : return button_E.isDown();
		case BUTTON_F : return button_F.isDown();
		case BUTTON_G : return button_G.isDown();
		case BUTTON_H : return button_H.isDown();
		case BUTTON_X : return button_X.isDown();
		case BUTTON_Y : return button_Y.isDown();
	}
	return false;
}




