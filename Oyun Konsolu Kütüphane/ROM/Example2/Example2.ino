#include "ROM.h"

void setup() {

  Serial.begin(115200);
 
  uint8_t zero[1024] = {};
  
  for(uint16_t i=0; i<8; i++){
    ROM::writeArray(i*1024, zero, 1024);
    Serial.println("One page write is done..!");
  }
  Serial.println("All pages write are done..!");
  
  for(uint16_t i=0; i<8; i++){
    ROM::readArray(i*1024, zero, 1024);

    for(uint16_t j=0; j<1024; j++){
      Serial.println(zero[j]);
    }
    Serial.println("One page read is done");
  }
   Serial.println("All pages read are done..!");
}

void loop() {
 

}