#include "Manager.h"
Manager manager;
TextBox myText;

void turn_on(EventArgs* e){
   digitalWrite(LED, HIGH);
}

void turn_off(EventArgs* e){
  digitalWrite(LED, LOW);
}

void setup(){
  manager.begin();

  myText.create(128, 8);
  Manager::scene.addChild(&myText);
  if(!myText.writeFromFile("a.h")){
     myText.write("Dosya yok!");
  }

  Manager::scene.addEventListener(BUTTON_DOWN, turn_on);
  Manager::scene.addEventListener(BUTTON_UP, turn_off);
}

void loop(){
  manager.run();
}