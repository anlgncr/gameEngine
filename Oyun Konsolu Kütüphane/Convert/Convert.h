#ifndef Convert_h
#define Convert_h
#include <Arduino.h>

class Convert{

	public :
		Convert();
		static uint16_t toUInt16(uint8_t*);
		static uint32_t toUInt32(uint8_t*);
	
};

#endif