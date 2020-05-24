#include <RAM.h>

void setup() {
  Serial.begin(9600);

  uint8_t* image = (uint8_t*)RAM::mallocStack(64);
  Serial.println((uint16_t)image);
  Serial.println((uint16_t)RAM::stack_pointer);
  
  uint8_t* image2 = (uint8_t*)RAM::mallocStack(128);
  Serial.println((uint16_t)image2);
  Serial.println((uint16_t)RAM::stack_pointer);
  
  RAM::popStack();
  Serial.println((uint16_t)RAM::stack_pointer);
  
  RAM::popStack();
  Serial.println((uint16_t)RAM::stack_pointer);
}

void loop() {
  // put your main code here, to run repeatedly:

}