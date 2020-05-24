#include "SerialHandler.h"
#include "RAM.h"

SerialHandler::SerialHandler(){	
	readStartTime = 0;
	cmd = NOOP;
	status = COMMAND;
	incomingData = (uint8_t*)RAM::malloc(128);	// 110 byte veri, 18 byte parametreler için
}

void SerialHandler::flushBuffer(){
	while(Serial.available()){
		Serial.read();
	}
}

void SerialHandler::processByte(const char data)
{
	static uint16_t dataPtr = 0;
	static uint16_t dataLength = 0;
	
	if(status != COMMAND){
		if(millis() > readStartTime + READ_TIMEOUT){
			status = COMMAND;
			flushBuffer();
		}
	}
	
	//--------------------------------------------->COMMAND
	if(status == COMMAND)
	{	
		if(cmd == NOOP){
			cmd = data;
		}
		else if(data == '\n')
		{
			if(cmd == OKAY){
				Serial.println(F("OK"));
				cmd = NOOP;
			}
			else if(cmd == HEY){
				Serial.println(F("Merhaba..."));
				cmd = NOOP;
			}
			else if(cmd == GET_RAM_USAGE){
				Serial.println((uint16_t)RAM::heap_pointer);
				cmd = NOOP;
			}
			else if(cmd == FORMAT){
				myFileHandler.formatMemory();
				Serial.println(F("Format tamamlandi."));
				cmd = NOOP;
			}
			else if(cmd == CHECK_ERROR){
				uint16_t errorCount = myFileHandler.checkFileError();
				Serial.write((uint8_t*)&errorCount, 2);
				cmd = NOOP;
			}
			else if(cmd == GET_FILE_COUNT){
				uint16_t fileCount = myFileHandler.getFileCount();
				Serial.write((uint8_t*)&fileCount, 2);
				cmd = NOOP;
			}
			else if(cmd == GET_FREE_SECTOR_COUNT){
				uint16_t sectorCount = myFileHandler.getFreeSectorCount();
				Serial.write((uint8_t*)&sectorCount, 2);
				cmd = NOOP;
			}
			else if(cmd == READ_SECTOR 	|| cmd == GET_FILE_ATTRIBUTE || cmd == START_WRITE || cmd == GET_ARB_SECTOR_ADD ||
					cmd == REMOVE_FILE 	|| cmd == NEW_FILE	|| cmd == RESIZE || cmd == RENAME)
			{
				readStartTime = millis();
				status = RECEIVE;
				dataPtr = 0;
				dataLength = 0;
			}
			else if(cmd == READ_DIRECTORY){
				uint16_t fileCount = myFileHandler.getFileCount();
				Serial.write((uint8_t*)&fileCount, 2);
				
				FileHandler::FILE currentFile;
				for(int i=0; i<512; i++){
					myFileHandler.getFileByIndex(i, &currentFile);
					if(currentFile.startSector != 0){
						Serial.write((uint8_t*)&currentFile, sizeof(FileHandler::FILE));
					}
				}
				cmd = NOOP;
			}
			else if(cmd == WRITE_NEXT){
				readStartTime = millis();
				status = RECEIVE;
			}
			else if(cmd == END_WRITE){
				uint16_t currentSector = RAM::read16(incomingData + 1);
				uint16_t length = RAM::read16(incomingData + 3);
				uint8_t isThisLastSector = RAM::read(incomingData + 5);
				
				uint16_t nextSector = myFileHandler.writeSector(incomingData + 6, currentSector, length, isThisLastSector);
				Serial.write((uint8_t*)&nextSector, 2);
			}
			else{ cmd = NOOP; }					
		}
		else{ cmd = data; }	
	}//--------------------------------------------> RECEIVE
    else if(status == RECEIVE)
	{
		readStartTime = millis();
		if(dataPtr == 0){
			dataLength = data;
			dataLength <<= 8;
			dataPtr++;
		}
		else if(dataPtr == 1){
			dataLength |= data;
			dataPtr++;
		}
		else if((dataPtr - 2) < dataLength){
			RAM::write(&incomingData[dataPtr - 2], data);
			dataPtr++;
			
			if(cmd == START_WRITE){
				if((dataPtr - 2) == 6){ //Parametreler geldiyse cevap ver
					Serial.print('O');
					status = COMMAND;
				}
			}
			else if(cmd == WRITE_NEXT){
				uint8_t dataSize = RAM::read(incomingData); // dataSize
				uint8_t addPtr = (dataPtr - 2) - 6; // veri öncesi parametreler için 6 byte ayrıldı
				if((addPtr % (dataLength - 6)) == 0 || (addPtr % dataSize) == 0){
					Serial.print('O');
					status = COMMAND;
				}
			}
		}
		else{
			status = COMMAND;
			if(cmd == READ_SECTOR){
				uint16_t currentSector = RAM::read16(incomingData);
				uint16_t length = RAM::read16(incomingData + 2);
				
				uint16_t nextSector = myFileHandler.readSector(incomingData, currentSector, length);
				uint8_t partCount = length / 55;
				uint8_t remainingPart = length % 55;

				RAM::startSeqRead(incomingData);
				for(uint8_t i=0 ;i<length; i++){
					Serial.write(RAM::readNext());
				}
				RAM::endSeqRead();
				Serial.write((uint8_t*)&nextSector, 2);
			}
			else if(cmd == NEW_FILE){
				char name[10];
				RAM::readArray(incomingData, name, 10);
				FileHandler::FILE newFile;
				uint8_t result = myFileHandler.createFile(name, &newFile, false);
				if(result == 'Y'){
					name[0] = 'Y';
					name[1] = newFile.fileAddress;
					name[2] = newFile.fileAddress >> 8;
					Serial.write(name, 3);
				}
				else{
					Serial.print(result); // TO DO --> Check here
				}
				/*else if(result == 'S'){ Serial.print('S'); }
				else if(result == 'N'){ Serial.print('N'); }
				else{ Serial.print(0x00); }*/
				
			}
			else if(cmd == REMOVE_FILE){
				uint16_t fileIndex = RAM::read16(incomingData);
				if(myFileHandler.removeFileByIndex(fileIndex)){
					Serial.print('O');//OK
				}
				else{ Serial.print('N'); }//NO OK	
			}
			else if(cmd == RESIZE){
				uint16_t fileIndex = RAM::read16(incomingData);
				uint16_t fileSize = RAM::read16(incomingData + 2);
			
				uint16_t newSize = myFileHandler.setFileSizeByIndex(fileIndex, fileSize);
				Serial.write((uint8_t*)&newSize, 2);
			}
			else if(cmd == RENAME){
				uint16_t fileIndex = RAM::read16(incomingData);
				char name[10];
				RAM::readArray(incomingData + 2, name, 10);
				
				myFileHandler.renameFileByIndex(fileIndex, name);
				Serial.write(0xFF);	
			}
			else if(cmd == GET_FILE_ATTRIBUTE){
				uint16_t fileIndex = RAM::read16(incomingData);
				FileHandler::FILE newFile = {};
				myFileHandler.getFileByIndex(fileIndex, &newFile);
				Serial.write((uint8_t*)&newFile, 16);
			}
			else if(cmd == GET_ARB_SECTOR_ADD){
				uint16_t fileIndex = RAM::read16(incomingData);
				uint16_t sectorIndex = RAM::read16(incomingData + 2);
				uint16_t sectorAddress = myFileHandler.getSectorAddressByIndex(fileIndex, sectorIndex);
				Serial.write((uint8_t*)&sectorAddress, 2);
			}
		}
	}
}
