#include "ROM.h"
#include "RAM.h"

ROM my_rom;

ROM::ROM(){
	Wire.begin();
};

uint8_t ROM::write(void* address, uint8_t data){
	Wire.beginTransmission(ROM_ADDRESS);
	Wire.write((uint8_t)((uint16_t)address >> 8));
	Wire.write((uint8_t)address);
	Wire.write(data);
	bool result = Wire.endTransmission();
	delay(5);
	
	if(result == 0)
		return true;
	else
		return false;
}

uint8_t ROM::read(void* address){
	Wire.beginTransmission(ROM_ADDRESS);
	Wire.write((uint8_t)((uint16_t)address >> 8));
	Wire.write((uint8_t)address);
	Wire.endTransmission();
	
	uint8_t data = 0;
	Wire.requestFrom(ROM_ADDRESS, 1);
	if (Wire.available()){	
		data = Wire.read();
	}
	return data;
}

uint16_t ROM::read16(void* address){
	uint16_t var;
	readArray(address, &var, 2);
	return var;
}

void ROM::write16(void* address, uint16_t data){
	writeArray(address, &data, 2);
}

void* ROM::readPtr(void* address){
	uint16_t var;
	readArray(address, &var, sizeof(void*));
	return (void*)var;
}

void ROM::writePtr(void* address, void* data){
	uint16_t ptr = (word)data;
	writeArray(address, &ptr, sizeof(void*));
}

uint32_t ROM::read32(void* address){
	uint32_t var;
	readArray(address, &var, 4);
	return var;
}

void ROM::write32(void* address, uint32_t data){
	writeArray(address, &data, 4);
}

void ROM::copyFromSpiRam(void* address, void* data, uint16_t length){
	writeBytes(address, data, length, 2);
}

void ROM::writeArray(void* address, void* data, uint16_t length){
	writeBytes(address, data, length , 1);
}

// length = 130 olsun
void ROM::writeBytes(void* address, void* data, uint16_t length, uint8_t memory){
	
	if(length == 0)
		return;
	
	uint16_t addressWord = (uint16_t) address;
	uint8_t* dataArray = (uint8_t*)data;
	
	
	uint8_t unAlignedLength = 0;
	
	/*	pageOffset: Adres sayfanın başlangıcından ne kadar ileride?
					Örnek : adres 290 ise 128'lik 2 sayfayı doldurmuş 280-256'dan 34 byte ileride olur.*/
	uint8_t pageOffset = addressWord % EEPROM_PAGE_SIZE; 
	
	if(pageOffset > 0){
		unAlignedLength = EEPROM_PAGE_SIZE - pageOffset; // 128 - 34 = 94 (İlk sayfada kalan alan)
		if(length < unAlignedLength){ //Alanın dışına çıkılmayacaksa
			unAlignedLength = length;
		}
		writePage(addressWord, dataArray, unAlignedLength, memory);
		length -= unAlignedLength; //94 byte ROM'a yazıldı bu yüzden uzunluktan çıkarıyoruz
	}	
	
	if(length > 0){	//Eğer geriye kalan sayfalar varsa devam et
		addressWord += unAlignedLength; //	yazılan byte kadar ileri gidiyoruz
		dataArray += unAlignedLength; 	//  yazılan byte kadar ileri gidiyoruz
		
		uint16_t pageCount = length / EEPROM_PAGE_SIZE; // Kaç adet sayfa kaldı
		for(uint8_t i=0; i<pageCount; i++){
			writePage(addressWord, dataArray, EEPROM_PAGE_SIZE, memory);
			addressWord += EEPROM_PAGE_SIZE;
			dataArray += EEPROM_PAGE_SIZE;
			length -= EEPROM_PAGE_SIZE;
		}
		
		if(length > 0){ // Geriye kaldıysa kalanları da yaz
			writePage(addressWord, dataArray, length, memory);
		}		
	}
}

void ROM::writePage(uint16_t address, uint8_t* data, uint16_t length, uint8_t memory){
	// Tam kapasitede 30 bytlık kaç buffer kullanılacak?
	uint8_t bufferCount = length / BUFFER_WRITE_SIZE; //94 bytlık veri için 3 tam kapasitede buffer gerek
	
	for(uint8_t i=0; i<bufferCount; i++){
		uint8_t nextBuffer = i * BUFFER_WRITE_SIZE;
		writeBuffer(address + nextBuffer, data + nextBuffer, BUFFER_WRITE_SIZE, memory);
	}
	
	//Geriye 4 byte kaldı
	uint8_t remainingBytes = length % BUFFER_WRITE_SIZE;
	uint8_t offset = length - remainingBytes; //90 byte ilerideyiz
	writeBuffer(address + offset, data + offset, remainingBytes ,memory); //Kalan 4 byte'ı yaz
}

void ROM::writeBuffer(uint16_t address, uint8_t* data, uint16_t length, uint8_t memory){	
	Wire.beginTransmission(ROM_ADDRESS);
	Wire.write((uint8_t)(address >> 8));
	Wire.write((uint8_t)address);
	
	if(memory == 1){
		for(uint8_t i=0; i<length; i++){
			Wire.write(data[i]);
		}
	}
	else if(memory == 2){
		RAM::startSeqRead(data);
		for(uint8_t i = 0; i < length; i++){
			Wire.write(RAM::readNext());
		}
		RAM::endSeqRead();
	}
	
	Wire.endTransmission();
	delay(5);
}

void ROM::copyToSpiRam(void* address, void* data, uint16_t length){
	readBytes(address, data, length, 2);
}

void ROM::readArray(void* address, void* data, uint16_t length){
	readBytes(address, data, length, 1);
}

void ROM::readBytes(void* address, void* data, uint16_t length, uint8_t memory){
	if(length == 0)
		return;
	
	uint16_t addressWord = (uint16_t)address;
	uint8_t* dataArray = (uint8_t*)data;
	
	uint8_t bufferCount = length / BUFFER_READ_SIZE;
    for(uint8_t i = 0; i < bufferCount; i++){
        uint16_t nextBuffer = i * BUFFER_READ_SIZE;
        readBuffer(addressWord + nextBuffer, data + nextBuffer, BUFFER_READ_SIZE, memory);
    }

    uint8_t remainingBytes = length % BUFFER_READ_SIZE;
    uint16_t offset = length - remainingBytes;
    readBuffer(addressWord + offset, data + offset, remainingBytes, memory);
}

void ROM::readBuffer(uint16_t address, uint8_t* data, uint16_t length, uint8_t memory){
	Wire.beginTransmission(ROM_ADDRESS);
    Wire.write((uint8_t)(address >> 8));
    Wire.write((uint8_t)address);
    Wire.endTransmission();
    Wire.requestFrom(ROM_ADDRESS, length);
	
	if(memory == 1){
		for(uint8_t i = 0; i < length; i++){
			if (Wire.available()){
				data[i] = Wire.read();
			}
		}
	}
	else if(memory == 2){
		RAM::startSeqWrite(data);
		for(uint8_t i = 0; i < length; i++){
			if (Wire.available()){
				RAM::writeNext(Wire.read());
			}
		}
		RAM::endSeqWrite();
	}
}
		




