#include <SerialHandler.h>


SerialHandler mySerial;
void setup() {
  Serial.begin(115200);
}

void loop() {
  while(Serial.available()){
    mySerial.processByte(Serial.read());
  }
}
