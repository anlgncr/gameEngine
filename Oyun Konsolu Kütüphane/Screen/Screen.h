#ifndef Screen_h
#define Screen_h
#include "arduino.h"
#include "SPI.h"
#include "RAM.h"
#include "DisplayObject.h"

#define COLS 128
#define ROWS 8
#define CS_SCREEN 0x03
#define DISP_OFF 0xAE
#define DISP_ON 0xAF
#define DISP_NOOP 0xE3 
#define MEM_ADD_MODE 0x20
#define HOR_ADD_MODE 0x00
#define PAGE_ADD_MODE 0x02
#define PAGE_START_ADD 0xB0
#define LCOL_START_ADD 0x00
#define HCOL_START_ADD 0x10
#define NORMAL_DIP 0xA6
#define INVERSE_DISP 0xA7
#define ENTIRE_DISP_ON 0xA5
#define RAM_CONTENT 0xA4
#define DISP_START_LINE 0x40
#define DISP_CLOCK 0xD5
#define SCAN_DIR_NORMAL 0xC0
#define SCAN_DIR_FLIPPED 0xC8
#define CONTRAST 0x81
#define CMD_LOCK 0xFD // 0x10 unlock, 0x14 lock
#define SEGMENT_REMAP 0xA0
#define DISP_OFFSET 0xD3
#define GPIO 0xDC

class Screen{
	public:
		Screen();
		void init();
		uint8_t testDraw(DisplayObject*);
		void draw();
		void writeBuffer();
		void clearBuffer();
		void setInverse(uint8_t);
		void setBrightness(uint8_t);
		void sleep();
		void wakeUp();
		void reset();
	
	private:
		uint8_t buf[ROWS][COLS];
		SPISettings spiSetting;
		
		void sendCommand(uint8_t);
		void sendData(uint8_t);
		void send(uint8_t);
		void sendDataArray(uint8_t*, uint16_t);
		void sendCommand2Byte(uint8_t, uint8_t);
		void readArrayPgm(uint8_t*, uint8_t*, uint16_t);
		void readArrayPgmReverse(uint8_t*, uint8_t*, uint16_t);
		
		struct object{
			DisplayObject *dispObj;
			uint8_t *image;
			uint16_t width;
			uint16_t height;
			int16_t x;
			int16_t y;
			uint8_t memory;
		}
		my_object;
		
};

#endif