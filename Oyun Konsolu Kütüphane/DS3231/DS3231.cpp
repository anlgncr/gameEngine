#include "DS3231.h"

DS3231::DS3231(){}

bool DS3231::setTime(uint8_t hours, uint8_t minutes, uint8_t seconds){
	Wire.beginTransmission(DS3231_ADDRESS);
	Wire.write(0x00);
	Wire.write(decToBcd(seconds));
	Wire.write(decToBcd(minutes));
	Wire.write(decToBcd(hours));

	bool result = Wire.endTransmission();
	
	if(result == 0) 
		return true;
	else
		return false;
}

bool DS3231::setDate(uint8_t day, uint8_t date, uint8_t month, uint8_t year){
	Wire.beginTransmission(DS3231_ADDRESS);
	Wire.write(0x03);
	Wire.write(decToBcd(day));
	Wire.write(decToBcd(date));
	Wire.write(decToBcd(month));
	Wire.write(decToBcd(year));

	bool result = Wire.endTransmission();
	
	if(result == 0) 
		return true;
	else
		return false;
}

void DS3231::getTime(dateTime* my_dt){
	Wire.beginTransmission(DS3231_ADDRESS);
	Wire.write(0x00);
	Wire.endTransmission();

	Wire.requestFrom(DS3231_ADDRESS, (uint8_t)3); 
	if (Wire.available()){
		my_dt->seconds = bcdToDec(Wire.read());
		my_dt->minutes = bcdToDec(Wire.read());
		my_dt->hours = bcdToDec(Wire.read());
	}
}

void DS3231::getDate(dateTime* my_dt){
	Wire.beginTransmission(DS3231_ADDRESS);
	Wire.write(0x03);
	Wire.endTransmission();

	Wire.requestFrom(DS3231_ADDRESS, (uint8_t)4); 
	if (Wire.available()){
		my_dt->day = bcdToDec(Wire.read());
		my_dt->date = bcdToDec(Wire.read());
		my_dt->month = bcdToDec(Wire.read());
		my_dt->year = bcdToDec(Wire.read());
	}
}

float DS3231::getTemperature(){
	Wire.beginTransmission(DS3231_ADDRESS);
	Wire.write(0x11);
	Wire.endTransmission();

	uint8_t upper, lower;

	Wire.requestFrom(DS3231_ADDRESS, (uint8_t)2); 
	if (Wire.available()){
		upper = Wire.read();
		lower = Wire.read();
	}

	if(upper >> 7) // 8 bit indicates that degree is under 0
		upper = upper & ~(0x80);

	return upper + (lower >> 6)* 0.25;
}

uint8_t DS3231::bcdToDec(uint8_t bcd){
	return (10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

uint8_t DS3231::decToBcd(uint8_t decimal){
	unsigned char tens = decimal / 10;
	unsigned char ones = decimal % 10;
	return (tens << 4) + ones;
}

