#include "ROM.h"

void setup() {

	Serial.begin(9600);
 
  uint8_t* adres = 120;
  uint8_t data[200];
  
  for(byte i=0; i<200; i++){
    data[i] = i;
  }
  
  uint8_t dataR[200];
  
  
  ROM::writeArray(adres, data, 200);
  ROM::readArray(adres, dataR, 200);
  
  for(byte i=0; i<200; i++){
    Serial.println(dataR[i]);
  }

}

void loop() {
 

}