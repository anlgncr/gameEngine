#include "Tween.h"

Tween newTween;
int a;
void setup() {
  Serial.begin(9600);
  Tween::tweenObject* tween = Tween::start(&a, 0, 100, 1000);
  Tween::setOnFinish(tween, finished);
}

void loop() {
  Tween::update();
  if(Tween::pool.indexOfTweens > 0){
    Serial.println(a);
  }
}

void finished(Tween::tweenObject* o){
  Serial.println("OK");
}
