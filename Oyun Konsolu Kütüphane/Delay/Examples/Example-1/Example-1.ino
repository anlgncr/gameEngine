#include "Delay.h"

void yaz(Delay::delayObject *d){
  Serial.println(Delay::pool.indexOfDelays);
  Serial.println(Delay::pool.indexOfUsedDelays);
}

Delay mdelay;
void setup() {
  Serial.begin(115200);
  Delay::delayObject *a = Delay::start(yaz, 500);
  Delay::setRepeatCount(a, 10);
}

void loop() {
  // put your main code here, to run repeatedly:
  Delay::update();
}