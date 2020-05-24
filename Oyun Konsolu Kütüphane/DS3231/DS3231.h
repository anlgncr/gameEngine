#ifndef DS3231_h
#define DS3231_h
#include "Arduino.h"
#include "Wire.h"

#define DS3231_ADDRESS 0x68

class DS3231{
	public:
		struct dateTime{
			uint8_t seconds;
			uint8_t minutes;
			uint8_t hours;
			uint8_t day;
			uint8_t date;
			uint8_t month;
			uint8_t year;
		};
		
		DS3231();
		float getTemperature();
		bool setTime(uint8_t, uint8_t, uint8_t);
		bool setDate(uint8_t, uint8_t, uint8_t, uint8_t);
		void getTime(dateTime*);
		void getDate(dateTime*);
	
	private:
		uint8_t bcdToDec(uint8_t);
		uint8_t decToBcd(uint8_t);
		uint8_t my_device_address;
	
};

#endif