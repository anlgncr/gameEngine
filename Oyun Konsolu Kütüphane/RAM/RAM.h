#ifndef RAM_h
#define RAM_h
#include <Arduino.h>
#include <SPI.h>

#define SMEM 0	// Static Memory
#define PGMEM 1 //Program Memory
#define SPIMEM 2 // SPI Memory

#define MEMORY_SIZE 32768

#define RAM_READ 0x03
#define RAM_WRITE 0x02

#define READ_STAT_REG 0x05
#define WRITE_STAT_REG 0x01

#define MODE_BYTE 0x00
#define MODE_SEQ 0x40
#define MODE_PAGE 0x80

#define CS_RAM1 1
#define CS_RAM2 2
#define DECODER_MASK 0xFC

class RAM{
		public:
		RAM();
		
		static void SPI_begin();
		static void SPI_end();
		static void selectMode(uint8_t);
		
		static uint8_t read(void*);
		static uint16_t read16(void*);
		static uint32_t read32(void*);
		
		static void* readPtr(void*);
		static void writePtr(void*, void*);
		
		static void write(void*, uint8_t);
		static void write16(void*, uint16_t);
		static void write32(void*, uint32_t);
		
		static void writeArray(void*, void*, uint16_t);
		static void writeArrayReverse(void*, void*, uint16_t);
		static void writeLoop(void*, uint8_t, uint16_t);
		static void readArray(void*, void*, uint16_t);
		static void readArrayReverse(void*, void*, uint16_t);
		
		static void startSeqRead(void*);
		static uint8_t readNext();
		static void endSeqRead();
		
		static void startSeqWrite(void*);
		static void writeNext(uint8_t);
		static void endSeqWrite();
		
		static void* malloc(uint16_t);
		static void* mallocStack(uint16_t);
		static void popStack();
		
		static uint8_t* heap_pointer;
		static uint8_t* stack_pointer;
		static SPISettings spiSetting;
};
#endif