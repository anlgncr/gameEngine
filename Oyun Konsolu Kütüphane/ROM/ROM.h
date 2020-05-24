#ifndef ROM_h
#define ROM_h
#include <Arduino.h>
#include <Wire.h>

#define ROM_ADDRESS 0x50
#define EEPROM_MEMORY_SIZE 0xFFFF
#define EEPROM_PAGE_SIZE 128
#define BUFFER_READ_SIZE 32
#define BUFFER_WRITE_SIZE 30

class ROM{
	public:
		ROM();
		
		static uint8_t read(void*);
		static uint8_t write(void*, uint8_t);
		
		static void readArray(void*, void*, uint16_t);
		static void writeArray(void*, void*, uint16_t);
		static void writeLoop(void*, uint8_t, uint16_t);
		
		static uint16_t read16(void*);
		static uint32_t read32(void*);
		
		static void* readPtr(void*);
		static void writePtr(void*, void*);
		
		static void write16(void*, uint16_t);
		static void write32(void*, uint32_t);
		
		static void copyToSpiRam(void*, void*, uint16_t);
		static void copyFromSpiRam(void*, void*, uint16_t);
		
	private:
		static void writeBytes(void*, void*, uint16_t, uint8_t);
		static void readBytes(void*, void*, uint16_t, uint8_t);
		static void writePage(uint16_t, uint8_t*, uint16_t, uint8_t);
		static void writeBuffer(uint16_t, uint8_t*, uint16_t, uint8_t);
		static void readBuffer(uint16_t, uint8_t*, uint16_t, uint8_t);
		
		
};
#endif