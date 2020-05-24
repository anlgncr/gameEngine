#include "FileHandler.h"

/*
	Toplam hafıza : 65536 bytes
	Ayırma boyutu : 112 bytes
	Azami dosya sayısı : 512 files
	Azami dosya boyutu : 55 Kilobytes
	Sektör boyutu : 110 bytes
	
	•Bu kütüphane 64kb'lık alana sahip hafıza için yazıldı.
	•Hafızanın ilk 8kb'lık kısmı directory için ayrıldı.
	•Geriye kalan 56 kb'lık alan ise 112 byte'lık sektörlere ayrıldı.
	•Toplam 512 sektör bulunuyor. Bu yüzden en fazla 512 dosya oluşturulabilir.
	•Her söktörün başında onu diğer sektörlere bağlamak için
	 bağlı bulunduğu sektörün adresi ve son sektörde LAST_SECTOR ibaresi bulunur. 
	•Dosya isimleri 10 karakteri geçmemeli, buna dosya adı ile uzantıyı ayıran nokta da dahil.
*/

FileHandler::FileHandler(){

}

uint16_t FileHandler::getSectorAddressByIndex(uint16_t fileIndex, uint16_t sectorIndex)
{
	FILE *currentFile = (FILE*)(fileIndex * sizeof(FILE));
	uint16_t currentSector = ROM::read16(&currentFile->startSector);
	
	for(uint16_t i=0; i<sectorIndex; i++){
		if(currentSector == LAST_SECTOR){
			currentSector = 0;
			break;
		}
		currentSector = ROM::read16(currentSector);
	}
	return currentSector;
}

uint16_t FileHandler::checkFileError(){
	uint16_t errorCount = 0;
	/*for(int i=0; i<MAX_FILE_COUNT; i++){
		int calSectorCount = getSectorCount(i);
		if(calSectorCount != 0){
			FILE *currentFile = (FILE*)(i * sizeof(FILE));
			uint16_t fileSize = ROM::read16(&currentFile->size);
			uint16_t sectorCount = fileSize / SECTOR_SIZE;
			
			if(sectorCount == 0){
				sectorCount++;
			}
			else if(fileSize % SECTOR_SIZE != 0){
				sectorCount++;
			}
			
			if(calSectorCount != sectorCount){
				errorCount++;
				ROM::write16(&currentFile->size, calSectorCount * SECTOR_SIZE);
			}
		}
	}*/
	return errorCount;
}

uint16_t FileHandler::getSectorCount(uint16_t fileIndex){
	FILE *currentFile = (FILE*)(fileIndex * sizeof(FILE));
	
	uint16_t startSector = ROM::read16(&currentFile->startSector);
	if(startSector == 0)
		return 0;
	
	uint16_t size = 1;
	uint16_t nextSector = ROM::read16(startSector);
	if(nextSector != LAST_SECTOR){
		do{
			size++;
			nextSector = ROM::read16(nextSector);
		}
		while(nextSector != LAST_SECTOR);
	}
	return size;
}

uint16_t FileHandler::getFileCount(){
	uint16_t fileCount = 0;
	for(uint16_t i=0; i<MAX_FILE_COUNT; i++)
	{
		FILE* currentFile = (FILE*)(i * sizeof(FILE));	
		uint16_t startSector = ROM::read16(&currentFile->startSector);
		
		if(startSector != 0){
			fileCount++;
		}
	}
	return fileCount;
}

uint16_t FileHandler::getFreeSectorCount(){
	uint16_t sectorCount = 0;
	for(uint16_t i=0; i<MAX_FILE_COUNT; i++)
	{
		FILE* currentFile = (FILE*)(i * sizeof(FILE));	
		uint16_t startSector = ROM::read16(&currentFile->startSector);
		
		if(startSector != 0){
			uint16_t fileSize = ROM::read16(&currentFile->size);
			sectorCount += fileSize / SECTOR_SIZE;
			sectorCount += (fileSize % SECTOR_SIZE) ? 0 : 1;
		}
	}
	return MAX_FILE_COUNT - sectorCount;
}

uint16_t FileHandler::readDirectory(uint8_t* data){
	int fileCount = 0;
	for(uint16_t i=0; i<MAX_FILE_COUNT; i++)
	{
		FILE* currentFile = (FILE*)(i * sizeof(FILE));	
		uint16_t startSector = ROM::read16(&currentFile->startSector);
		
		if(startSector != 0){
			ROM::copyToSpiRam(currentFile, data + fileCount * sizeof(FILE), sizeof(FILE));
			fileCount++;
		}
	}
	return fileCount;
}

void FileHandler::fileToSpiRam(FILE* file, uint8_t* data, uint16_t length){
	readFile(file, data, length, 2);
}

void FileHandler::fileToSram(FILE* file, uint8_t* data, uint16_t length){
	readFile(file, data, length, 1);
}

uint16_t FileHandler::readSector(uint8_t* data, uint16_t currentAddress, uint16_t length){
	if(!currentAddress || length < 1){
		return 0;
	}
	ROM::copyToSpiRam(currentAddress + 2, data, length);
	return ROM::read16(currentAddress);//next sector
}

uint16_t FileHandler::writeSector(uint8_t* data, uint16_t currentSector, uint16_t length, uint8_t sector){
	if(currentSector < DATA_START_ADD){
		return 0;
	}
	ROM::copyFromSpiRam(currentSector + 2, data, length);//Veriyi RAM'den ROM'a kopyala
	
	if(sector == LAST_SECTOR){ // Eğer bu son sektör ise sektörü son sektör olarak işaretle
		freeSectors(currentSector); // Kullanılmayan alanları serbest bırak
		return LAST_SECTOR; // Doğrulama için son sektör işaretini döndür 			
	}
	else{
		uint16_t nextSector = ROM::read16(currentSector);
		if(nextSector == LAST_SECTOR){
			nextSector = findFreeSector((currentSector - DATA_START_ADD)/112 + 1);
			if(nextSector == 0){
				return 0;
			}
			ROM::write16(currentSector, nextSector);
			ROM::write16(nextSector, LAST_SECTOR);
		}
		return nextSector;
	}
}

uint8_t FileHandler::getFile(char* fileName, FILE* file){	
	if(fileName[0] == '\0')
		return false;
	
	for(uint16_t i=0; i<MAX_FILE_COUNT; i++)
	{
		FILE* currentFile = (FILE*)(i * sizeof(FILE));	
		char name[NAME_SIZE];
		ROM::readArray(&currentFile->name, name, NAME_SIZE);
		
		if(compareName(name, fileName)){
			if(file != NULL){
				file->fileAddress = i * sizeof(FILE);
				file->startSector = ROM::read16(&currentFile->startSector);
				file->size = ROM::read16(&currentFile->size);
			}
			return true;
			
		}
	}
	return false;
}

uint8_t FileHandler::getFileByIndex(uint16_t fileIndex, FILE* file){
	if(fileIndex >= MAX_FILE_COUNT){
		return false;
	}
	
	uint8_t* fileAddress = fileIndex * sizeof(FILE);
	
	ROM::readArray(fileAddress, file, sizeof(FILE));
	return true;
}

void FileHandler::renameFileByIndex(uint16_t fileIndex, uint8_t* name){
	ROM::writeArray(fileIndex * sizeof(FILE), name, 10);
}

uint16_t FileHandler::setFileSizeByIndex(uint16_t fileIndex, uint16_t size){
	FILE* currentFile = (FILE*)(fileIndex * sizeof(FILE));
	ROM::write16(&currentFile->size, size);
	return ROM::read16(&currentFile->size);
}

uint8_t FileHandler::createFile(char* fileName, FILE* file){
	return createFile(fileName, file, true);
}

uint8_t FileHandler::createFile(char* fileName, FILE* file, bool checkFileName){
	if(fileName[0] == '\0')
		return 0; 
	
	if(checkFileName && getFile(fileName, file)){
		return 'S'; //same name
	}
	
	for(uint16_t i=0; i<MAX_FILE_COUNT; i++){
		uint16_t fileAddress = i * sizeof(FILE);
		FILE* currentFile = (FILE*)(fileAddress);
		
		uint16_t startSector = ROM::read16(&currentFile->startSector);
		if(startSector == 0){
			file->startSector = findFreeSector(0);
			if(file->startSector == 0){
				return 'N'; //no place
			}
			file->fileAddress = fileAddress;
			file->size = 0;
			
			FILE tempFile = {};
			for(uint8_t i=0; i<NAME_SIZE; i++){
				tempFile.name[i] = fileName[i];
				
				if(fileName[i] == '\0'){
					break;
				}
			}
			tempFile.fileAddress = fileAddress;
			tempFile.startSector = file->startSector;
			tempFile.size = 0;
			
			ROM::writeArray((uint8_t*)fileAddress, (uint8_t*)&tempFile, sizeof(FILE));
			ROM::write16(file->startSector, LAST_SECTOR);
			return 'Y'; // yes
		}
	}
	//Serial.println("There is no empty place!");
	return false;
}

uint8_t FileHandler::getNameLength(char* fileName){
	uint8_t length = 0;
	for(uint8_t i=0; i<NAME_SIZE; i++){
		if(fileName[i] == '\0'){
			return length;
		}
		else{
			length++;
		}
	}
	return length;
}

uint8_t FileHandler::writeFile(FILE* file, uint8_t* data, uint16_t length){
	uint16_t currentSector = file->startSector; // Dosya yazımına başlanacak ilk sektor
	if(!currentSector)
		return false;
	
	uint16_t sectorCount = (length / SECTOR_SIZE); // Verinin yazılıcağı sektor sayısı
	uint8_t remaningData = length % SECTOR_SIZE; // Son sektore yazılacak veri miktarı
	
	uint16_t sectorIndex = 0;
	if(sectorCount > 0){
		for(sectorIndex = 0; sectorIndex < sectorCount; sectorIndex++){
			ROM::writeArray(currentSector + 2, data + sectorIndex * SECTOR_SIZE, SECTOR_SIZE);//Sektorun ilk 2 byte'ında sıradaki sektorun adresi var
		
			if(!(sectorIndex == (sectorCount - 1) && remaningData == 0))
			{	
				uint16_t nextSector = ROM::read16(currentSector);
				if(nextSector == LAST_SECTOR){
					nextSector = findFreeSector((currentSector - DATA_START_ADD)/112 + 1);
					if(!nextSector)
						return false;
						
					ROM::write16(currentSector, nextSector);//Şimdiki sektorun ilk 2 byte'ı sıradaki sektoru göstersin
					ROM::write16(nextSector, LAST_SECTOR);//Sıradaki sektoru şimdilik son sektor olarak işaretle
				}
				currentSector = nextSector;
			}
		}
	}
	ROM::writeArray(currentSector + 2, data + sectorIndex * SECTOR_SIZE, remaningData);
	
	file->size = length;
	FILE *currentFile = file->fileAddress;
	ROM::write16(&currentFile->size, length);
	
	//Eğer veri önceki veriden daha az sektor kaplıyor ise diğer sektorleri serbest bırak
	freeSectors(currentSector);
	//Serial.println("Write operation is complete");
	return true;
}

uint8_t FileHandler::removeFile(FILE* file){
	uint16_t currentSector = file->startSector;
	if(!currentSector)
		return false;
	
	uint16_t nextSector;
	do{
		nextSector = ROM::read16(currentSector);
		ROM::write16(currentSector, FREE_SECTOR);
		currentSector = nextSector;
	}
	while(nextSector != LAST_SECTOR);
	
	FILE emptyFile = {};		
	ROM::writeArray((uint8_t*)file->fileAddress, (uint8_t*)&emptyFile, sizeof(FILE));
	
	*file = {};
	//Serial.println("File has been deleted");
	return true;
}

uint8_t FileHandler::removeFileByIndex(uint16_t fileIndex){
	if(fileIndex >= MAX_FILE_COUNT){
		return false;
	}
	
	FILE file;
	FILE* currentFile = (FILE*)(fileIndex * sizeof(FILE));
	
	ROM::readArray(currentFile, &file, sizeof(FILE));
	return removeFile(&file);
}

void FileHandler::formatMemory(){
	FILE emptyFile = {};	
	
	for(uint16_t i=0; i<MAX_FILE_COUNT; i++){
		ROM::writeArray((uint8_t*)(i * sizeof(FILE)), (uint8_t*)&emptyFile, sizeof(FILE));
	}

	for(uint32_t i=DATA_START_ADD; i<ROM_MEMORY_SIZE; i+=(SECTOR_SIZE + 2)){
		ROM::write16(i, FREE_SECTOR);	
	}
}
	
/*-------------------------------------------------> PRIVATE*/
uint8_t FileHandler::compareName(char* name1, char* name2){
	for(uint8_t i=0; i<NAME_SIZE; i++){
		if(name1[i] == name2[i]){
			if(name1[i] == '\0'){
				return true;
			}
		}
		else{
			return false;
		}
	}
	return true;
}

uint8_t FileHandler::readFile(FILE* file, uint8_t* data, uint16_t length, uint8_t memory){
	uint16_t currentSector = file->startSector;
	if(!currentSector)
		return false;

	uint16_t currentSize = file->size;
	if(length > currentSize){
		length = currentSize;
	}
	
	uint16_t sectorCount = (length / SECTOR_SIZE);
	uint8_t remaningData = length % SECTOR_SIZE;
	uint16_t sectorIndex = 0;
	if(sectorCount > 0){
		for(sectorIndex = 0; sectorIndex < sectorCount; sectorIndex++){
			if(memory == 1){
				ROM::readArray(currentSector + 2, data + sectorIndex * SECTOR_SIZE, SECTOR_SIZE);
			}
			else if(memory == 2){
				ROM::copyToSpiRam(currentSector + 2, data + sectorIndex * SECTOR_SIZE, SECTOR_SIZE);
			}
			currentSector = ROM::read16(currentSector);
		}
	}
	if(memory == 1){
		ROM::readArray(currentSector + 2, data + sectorIndex * SECTOR_SIZE, remaningData);
	}
	else if(memory == 2){
		ROM::copyToSpiRam(currentSector + 2, data + sectorIndex * SECTOR_SIZE, remaningData);
	}
	return true;
}

uint16_t FileHandler::findFreeSector(uint16_t sectorIndex){	
	if(sectorIndex > 511){
		sectorIndex = 0;
	}
	
	uint16_t startAdd = sectorIndex * 112 + DATA_START_ADD;
	
	for(uint32_t address=startAdd; address<ROM_MEMORY_SIZE; address+=(SECTOR_SIZE + 2)){
		uint16_t nextAdd = ROM::read16(address);
		if(nextAdd == 0){
			return address;
		}
	}

	uint16_t remainingSectors = startAdd - DATA_START_ADD;
	for(uint32_t address = DATA_START_ADD; address<(DATA_START_ADD + remainingSectors); address+=(SECTOR_SIZE + 2)){
		uint16_t nextAdd = ROM::read16(address);
		if(nextAdd == 0){
			//Serial.println("Rollovered...!");
			return address;
		}
	}
	return 0;
}

void FileHandler::freeSectors(uint16_t currentSector)
{
	uint16_t nextSector = ROM::read16(currentSector);
	if(nextSector != LAST_SECTOR){
		ROM::write16(currentSector, LAST_SECTOR);
		do{
			currentSector = nextSector;
			nextSector = ROM::read16(currentSector);
			ROM::write16(currentSector, FREE_SECTOR);	
		}
		while(nextSector != LAST_SECTOR);
	}
}
