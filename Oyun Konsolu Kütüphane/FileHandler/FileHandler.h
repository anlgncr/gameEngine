#ifndef FileHandler_h
#define FileHandler_h

#include "Arduino.h"
#include "ROM.h"
#include "RAM.h"

#define SECTOR_SIZE 110 // +2 gösterge adresi için
#define DATA_START_ADD 0x2000
#define MAX_FILE_COUNT 512
#define ROM_MEMORY_SIZE 65535

#define LAST_SECTOR 0xFF
#define FREE_SECTOR 0x00
#define NAME_SIZE 10 //Don't change this value

class FileHandler
{
	public:	
		typedef struct{
			char name[NAME_SIZE];
			uint16_t fileAddress;
			uint16_t startSector;
			uint16_t size;
		}FILE;
		
		
		/*
			Constructor
		*/
		FileHandler();
		
		uint16_t checkFileError();
		uint16_t getSectorCount(uint16_t fileIndex);
		
		uint16_t getSectorAddressByIndex(uint16_t fileIndex, uint16_t sectorIndex);
		
		/*
			Dosyanın adını RAM'den aldığı 10 bytelık veriyle değiştirir.
		*/
		void renameFileByIndex(uint16_t fileIndex, uint8_t* name);
		
		/*
			fileName : dosyanın adı (SRAM)
			Verilen isimle yeni bir dosya oluşturur. Dosya adı 10 karakteri geçmemeli(Nokta da dahil).
			Örnek yeni.text
		*/
		uint8_t createFile(char* fileName, FILE* file);
		uint8_t createFile(char* fileName, FILE* file, bool checkFileName);
		
		/*
			Dosyanın directory'deki verilerini sıfırlar.
			Dosyanın kullandığı sektörleri serbest bırakır.
		*/
		uint8_t removeFile(FILE* file);
		
		/*
			Dosyanın directory'deki bilgilerini döndürür.
		*/
		uint8_t getFile(char* fileName, FILE* file);
			
		/*
			Dosyanın boyutunu değiştirir.
		*/
		uint16_t setFileSizeByIndex(uint16_t fileIndex, uint16_t size);
		
		/*
			Dosyayı directory'deki sırasına göre döndürür.
			Toplam 512 dosya var(Directory boyutu 8.192 bytes, bir dosya 16 bytes). 
		*/
		uint8_t getFileByIndex(uint16_t fileIndex, FILE* file);
		
		/*
			Dosyayı directory'deki sırasına göre siler.
		*/
		uint8_t removeFileByIndex(uint16_t fileIndex);
		
		/*
			data : yazılacak olan verinin adresi (SRAM)
			length : Dosyanın veri uzunluğu
			Belirlenen uzunluk kadar veriyi dosyaya yazar.
		*/
		uint8_t writeFile(FILE* file, uint8_t* data, uint16_t length);
		
		/*
			currentSector : Verinin okunacağı sektör.
			length : Dosyanın veri uzunluğu(En fazla 110 bytes olmalı)
			Dosyaya ait bir sektörü okur.
		*/
		uint16_t readSector(uint8_t* data, uint16_t currentSector, uint16_t length);
		
		/*
			currentSector : Üzerine veri yazılacak sektör
			length : Dosyanın veri uzunluğu(En fazla 110 bytes olmalı)
			sector : Dosyanın son sektörü yazılacaksa değer LAST_SECTOR olur.
			Dosyaya ait sektöre veri yazar.
		*/
		uint16_t writeSector(uint8_t* data, uint16_t currentSector, uint16_t length, uint8_t sector);
		
		/*
			data : SPI RAM'e ait adres
			Directory'deki tüm verileri SPI RAM'e yazar. 
		*/
		uint16_t readDirectory(uint8_t* data);
		
		/*
			data : SPI RAM adresi
			length : dosyanın veri uzunluğu
			Dosyayı ROM'dan SPI RAM'e yazar.
		*/
		void fileToSpiRam(FILE* file, uint8_t* data, uint16_t length);
		
		/*
			data : SRAM adresi
			length : dosyanın veri uzunluğu
			Dosyayı ROM'dan SRAM'e yazar.
		*/
		void fileToSram(FILE* file, uint8_t* data, uint16_t length);
		
		/*
			Directory'de bulunan toplam dosya sayısını verir.
		*/
		uint16_t getFileCount();
		
		/*
			Hafızada kullanılan toplam sektör sayısını verir.
		*/
		uint16_t getFreeSectorCount();
		
		/*
			Directory'yi sıfırlar. Tüm sektörleri serbest bırakır.
		*/
		void formatMemory();
		
		/*
			Dosya adının uzunluğunu döndürür. Eğer dosya adı 10' dan büyükse 10 döndürür.
		*/
		uint8_t getNameLength(char* fileName);
		
	private:
		
		/*
			İki dosya adı eşitse true döndürür.
		*/
		uint8_t compareName(char* name1, char* name2);
		
		/*
			Dosyanın verilerini belirtilen hafızaya yazar.
		*/
		uint8_t readFile(FILE* file, uint8_t* data, uint16_t length, uint8_t memory);
		
		/*
			Serbest durumda bir sektör arar. Bulursa sektörün adresini döndürür, bulamazsa
			0 döndürür.
		*/
		uint16_t findFreeSector(uint16_t startAdd);
		
		/*
			currentSector : Dosyanın kullandığı son sektör. Bu sektörden sonraki sektörleri
			eğer kullanımda ise onları serbest bırakır.
		*/
		void freeSectors(uint16_t currentSector);
			
};
#endif
