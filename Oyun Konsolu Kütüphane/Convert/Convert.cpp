#include "Convert.h"

Convert::Convert(){}

uint16_t Convert::toUInt16(uint8_t* number){
	uint16_t value = number[1];
	value <<= 8;
	value |= number[0];
	return value;
}

uint32_t Convert::toUInt32(uint8_t* number){
	uint32_t value = number[3];
	value <<= 8;
	value |= number[2];
	value <<= 8;
	value |= number[1];
	value <<= 8;
	value |= number[0];
	return value;
}