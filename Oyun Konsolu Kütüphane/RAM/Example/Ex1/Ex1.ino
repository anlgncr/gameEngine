#include <RAM.h>


void setup() {
  Serial.begin(9600);

  byte *v = (byte*)RAM::malloc(sizeof(byte));
  RAM::write(v, 255);

  Serial.print("Adres =>");
  Serial.print((word)v);
  Serial.print("   Deger =>");
  Serial.println(RAM::read(v));

  word *a = (word*)RAM::malloc(sizeof(word));
  RAM::write16(a, 65535);

  Serial.print("Adres =>");
  Serial.print((word)a);
  Serial.print("   Deger =>");
  Serial.println(RAM::read16(a));

  unsigned long *b = (unsigned long*)RAM::malloc(sizeof(unsigned long));
  RAM::write32(b, 123456789);

  Serial.print("Adres =>");
  Serial.print((word)b);
  Serial.print("   Deger =>");
  Serial.println(RAM::read32(b));

  for(int i=0; i<1024; i+=2){
    RAM::write16((void*)i, i);
  }

  for(int i=0; i<1024; i+=2){
    Serial.print(i);
    Serial.print(" => ");
    word data = RAM::read16((void*)i);
    
    if(i != data)
      Serial.print(" HATA ");
      
    Serial.println(data);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}