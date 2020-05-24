#ifndef SerialHandler_h
#define SerialHandler_h
#include "ROM.h"
#include "RAM.h"
#include "FileHandler.h"
#define READ_TIMEOUT 2000


class SerialHandler{
	
	public:
		SerialHandler();

		void flushBuffer();
		void processByte(const char data);
		
		typedef enum {
			NOOP = '\0',
			OKAY = 'O', 
			HEY = 'H',
			RESIZE = 'Z',
			RENAME = 'A',

			START_WRITE = 'Y',
			WRITE_NEXT = 'X',
			END_WRITE = 'W',
			
			//WRITE_OBJECTS_INFO = 'I';
			
			READ_SECTOR = 'S',
			READ_DIRECTORY = 'D', 
			NEW_FILE = 'N',
			REMOVE_FILE = 'R',
			GET_FILE_COUNT = 'C',
			GET_ARB_SECTOR_ADD = 'B', //ARBITRARY SECTOR ADDRESS, Bir dosyanın istenen sektör adresini verir
			GET_FREE_SECTOR_COUNT = 'U',
			GET_FILE_ATTRIBUTE = 'T',
			GET_RAM_USAGE = 'G',
			FORMAT = 'F',
			CHECK_ERROR = 'E'
		}
		commands;

		typedef enum {NONE, COMMAND, TRANSMIT, RECEIVE} serialStat;
		
		private:
			FileHandler myFileHandler;
			uint8_t* incomingData;
			uint32_t readStartTime;
			commands cmd;
			serialStat status;
};
#endif
