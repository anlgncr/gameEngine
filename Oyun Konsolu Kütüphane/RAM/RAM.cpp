#include "RAM.h"
uint8_t* RAM::heap_pointer = (uint8_t*)0x0001;
uint8_t* RAM::stack_pointer = (uint8_t*)0x7FFF;

SPISettings RAM::spiSetting = SPISettings(16000000, MSBFIRST, SPI_MODE0);
RAM my_spi_ram;

RAM::RAM(){
	SPI.begin();
	
	pinMode(A0, OUTPUT);//Decoder
	pinMode(A1, OUTPUT);
	
	selectMode(MODE_SEQ);
};

void RAM::SPI_begin(){
	SPI.beginTransaction(spiSetting);
	
	PORTC &= DECODER_MASK;
	PORTC |= CS_RAM1;
}

void RAM::SPI_end(){
	//PORTD |= (1<<RAM_CS);
	//digitalWrite(RAM_CS, HIGH);
		
	SPI.endTransaction();
}

void RAM::selectMode(uint8_t mode){
	/*if(mode != MODE_BYTE || mode != MODE_PAGE || mode != MODE_SEQ)
		return;*/
	
	SPI_begin();
	SPI.transfer(WRITE_STAT_REG);
	SPI.transfer(mode);
	SPI_end();
}

void RAM::write(void *address, uint8_t data){
	//selectMode(MODE_BYTE);
	
	SPI_begin();
	SPI.transfer(RAM_WRITE);
	SPI.transfer16((uint16_t)address);
	SPI.transfer(data);
	SPI_end();
}

uint8_t RAM::read(void *address){
	//selectMode(MODE_BYTE);
	
	SPI_begin();
	SPI.transfer(RAM_READ);
	SPI.transfer16((uint16_t)address);
	uint8_t data = SPI.transfer(0);
	SPI_end();
	
	return data;
}

void RAM::startSeqWrite(void* address){
	selectMode(MODE_SEQ);
	SPI_begin();
	SPI.transfer(RAM_WRITE);
	SPI.transfer16((uint16_t)address);	
}

void RAM::writeNext(uint8_t data){
	SPI.transfer(data);
}

void RAM::endSeqWrite(){
	SPI_end();
}

void RAM::startSeqRead(void* address){
	selectMode(MODE_SEQ);
	SPI_begin();
	SPI.transfer(RAM_READ);
	SPI.transfer16((uint16_t)address);	
}

uint8_t RAM::readNext(){
	return SPI.transfer(0);
}

void RAM::endSeqRead(){
	SPI_end();
}

void RAM::writeLoop(void* address, uint8_t data, uint16_t length){
	//selectMode(MODE_SEQ);
	
	SPI_begin();
	SPI.transfer(RAM_WRITE);
	SPI.transfer16((uint16_t)address);	

	for(uint16_t i=0; i<length; i++){
		SPI.transfer(data);
	}
	SPI_end();
}

void RAM::writeArray(void* address, void* data, uint16_t length){
	//selectMode(MODE_SEQ);	
	SPI_begin();
	SPI.transfer(RAM_WRITE);
	SPI.transfer16((uint16_t)address);	
	uint8_t *data_array = (uint8_t*)data;

	for(uint16_t i=0; i<length; i++){
		SPI.transfer(data_array[i]);
	}
	SPI_end();
}

void RAM::writeArrayReverse(void* address, void* data, uint16_t length){	
	SPI_begin();
	SPI.transfer(RAM_WRITE);
	SPI.transfer16((uint16_t)address);	
	uint8_t *data_array = (uint8_t*)data;

	for(uint16_t i=0; i<length; i++){
		SPI.transfer(data_array[length - i - 1]);
	}
	SPI_end();
}

void RAM::readArray(void* address, void* data, uint16_t length){
	//selectMode(MODE_SEQ);
	
	SPI_begin();
	SPI.transfer(RAM_READ);
	SPI.transfer16((uint16_t)address);	
	uint8_t *data_array = (uint8_t*)data;

	for(uint16_t i=0; i<length; i++){
		data_array[i] = SPI.transfer(0);
	}
	SPI_end();
}

void RAM::readArrayReverse(void* address, void* data, uint16_t length){
	SPI_begin();
	SPI.transfer(RAM_READ);
	SPI.transfer16((uint16_t)address);	
	uint8_t *data_array = (uint8_t*)data;

	for(int16_t i=length-1; i>=0; i--){
		data_array[i] = SPI.transfer(0);
	}
	SPI_end();
}

void RAM::write16(void* address, uint16_t data){
	writeArray(address, &data, 2);
}

void RAM::writePtr(void* address, void* data){
	uint16_t ptr = (word)data;
	writeArray(address, &ptr, sizeof(void*));
}

void RAM::write32(void* address, uint32_t data){
	writeArray(address, &data, 4);
}

uint16_t RAM::read16(void* address){
	uint16_t var;
	readArray(address, &var, 2);
	return var;
}

void* RAM::readPtr(void* address){
	uint16_t var;
	readArray(address, &var, sizeof(void*));
	return (void*)var;
}

uint32_t RAM::read32(void* address){
	uint32_t var;
	readArray(address, &var, 4);
	return var;
}


//-----------------------------> Memory Allocation

void* RAM::malloc(uint16_t size){
	if(size == 0)
		return NULL;
	
	uint16_t current_heap_size = heap_pointer + size;
	if(current_heap_size >= MEMORY_SIZE)
		return NULL;
	
	uint8_t* temp_pointer = heap_pointer;
	writeLoop(heap_pointer, 0, size);
	heap_pointer +=size;
	
	return temp_pointer;
}

void* RAM::mallocStack(uint16_t size){
	stack_pointer -= (size + 2);
	writeLoop(stack_pointer, 0, size);
	write16(stack_pointer, size);
	
	return stack_pointer + 2;
}

void RAM::popStack(){
	if(stack_pointer == 0x7FFF)
		return;
	
	stack_pointer += (read16(stack_pointer) + 2);	
}


