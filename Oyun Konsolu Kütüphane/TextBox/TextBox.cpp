#include "TextBox.h"
#include "Manager.h"
#include "MovieClip.h"

TextBox::TextBox(uint8_t childSize) : DisplayObject(childSize + 1){}
TextBox::TextBox() : DisplayObject(1){}

bool TextBox::create(uint16_t width, uint16_t rowCount){
	return createTextBox(width, rowCount, TYPE_8);
}

bool TextBox::create(uint16_t width, uint16_t rowCount, uint8_t type){
	return createTextBox(width, rowCount, type);
}

bool TextBox::createTextBox(uint16_t width, uint16_t rowCount, uint8_t type)
{
	if(type > TYPE_16)
		type = TYPE_8;
	
	uint8_t *image = (uint8_t*)RAM::malloc(width * rowCount * type + 4);
	if(!image) { return false; }
	
	my_object = (object*)RAM::malloc(sizeof(object));
	if(!my_object) { return false; }
	
	RAM::write16(&image[0], width);
	RAM::write16(&image[2], rowCount * type * 8);
	
	setMemory(SPIMEM);
	setImage(image);
	setMaskType(IMAGE_AUTO_MASK);
	setImageIndex(&image[4]);

	setCharset(type);
	return true;
}

void TextBox::write(char* text){
	writeToTextBox(text, SMEM);
}

void TextBox::writeFromProgmem(char* text){
	writeToTextBox(text, PGMEM);
}

bool TextBox::writeFromFile(char *fileName){
	FileHandler::FILE newFile;
	bool fileExists = Manager::fileHandler.getFile(fileName, &newFile);
	if(fileExists && newFile.size > 0)
	{
		uint16_t text_length = newFile.size - 7; //4 font parametreleri için
		uint8_t* fileText = RAM::mallocStack(newFile.size); //+1 null terminator
		Manager::fileHandler.fileToSpiRam(&newFile, fileText, newFile.size);
		uint8_t* text = RAM::mallocStack(getUTF8CharCount(&fileText[7], newFile.size - 7));
		decodeUTF8(text, &fileText[7], newFile.size - 7);
		setCharset(RAM::read(&fileText[6]) + 1); //index sıfırdan başladığı için +1
		writeToTextBox(text, SPIMEM);
		RAM::popStack();
		RAM::popStack();
		return true;
	}
	return false;
}

void TextBox::decodeUTF8(uint8_t* text, uint8_t* UTF8_encodedText, uint16_t length){
	uint16_t charIndex = 0;
	while(charIndex < length)
	{
		uint8_t firstByte = RAM::read(UTF8_encodedText);
		
		uint8_t byteCount = 0;
		for(uint8_t i=0; i<5; i++){
			if(firstByte & (0x80 >> i)){
				byteCount++;
			}
			else{
				break;
			}
		}
		byteCount += (byteCount == 0)? 1 : 0;
		
		if(byteCount == 2)
		{
			uint16_t unicodedChar = firstByte & 0x1F;
			unicodedChar <<= 6;
			uint8_t secondByte = RAM::read(UTF8_encodedText + 1) & 0x3F;
			unicodedChar |= secondByte;
			
			uint8_t newVal = 0x8D; // karakter setinin sonundaki özel karakter
			switch(unicodedChar){
				case 0x00E7: newVal = 0x81; break; //ç
				case 0x011F: newVal = 0x82; break; //ğ
				case 0x0131: newVal = 0x83; break; //ı
				case 0x00F6: newVal = 0x84; break; //ö
				case 0x015F: newVal = 0x85; break; //ş
				case 0x00FC: newVal = 0x86; break; //ü
				case 0x00C7: newVal = 0x87; break; //Ç
				case 0x0130: newVal = 0x88; break; //İ
				case 0x011E: newVal = 0x89; break; //Ğ
				case 0x00D6: newVal = 0x8A; break; //Ö
				case 0x015E: newVal = 0x8B; break; //Ş
				case 0x00DC: newVal = 0x8C; break; //Ü
			}                                        
			RAM::write(text++, newVal);
		}
		else if(byteCount > 2){
			RAM::write(text++, 0x8D);
		}
		else{
			RAM::write(text++, firstByte);
			//Serial.println((char)firstByte);
		}
		UTF8_encodedText += byteCount;
		charIndex +=byteCount;
	}
}

uint16_t TextBox::getUTF8CharCount(uint8_t* text, uint16_t length){
	uint16_t charCount = 0;
	uint16_t charIndex = 0;
	while(charIndex < length)
	{
		uint8_t firstByte = RAM::read(text);
		uint8_t byteCount = 0;
		for(uint8_t i=0; i<5; i++){
			if(firstByte & (0x80 >> i)){
				byteCount++;
			}
			else{ break; }
		}
		byteCount += (byteCount == 0)? 1 : 0;
		
		text += byteCount;
		charIndex += byteCount;
		charCount++;
	}
	return charCount;
}

void TextBox::writeToTextBox(char *text, uint8_t textSource){
	A:
	Charset* charset = getCharset();
	
	uint8_t row_width = getCanvasWidth(); // Satır genişliği (pixel)
	uint8_t char_width = charset->getCharWidth(); // Bir karakterin genişliği (pixel) + boşluk
	uint8_t char_height = charset->getCharHeight();
	uint8_t row_char_count = row_width / (char_width + 1); // Bir satırda olabilecek azami karakter sayısı
	uint8_t row_remaining_space = row_width % (char_width + 1); // Satırın sonunda artan boşluk
	
	uint8_t row_count = getCanvasHeight() / char_height; // Toplam satır sayısı
	uint16_t char_count = getTextIndex(); // Toplam karakter sayısı
	uint8_t *image = getImageIndex(); // görüntü dizisinin kaldığı son index
	uint8_t start_col = char_count % row_char_count; // Son yazının kaldığı sütun
	uint8_t start_row = char_count / row_char_count; // Son yazının bittiği satır
	
	uint16_t charsetWidth = charset->getWidth();
	uint16_t charsetCharCountH = charsetWidth / char_width; //charset teki yatay karakter sayısı
	
	uint8_t (*charsetImage)[char_width] = (uint8_t(*)[char_width])charset->getImage();
	
	if(start_row >= row_count){
		clear();
		goto A;
	}
	
	uint8_t nextChar;
	uint8_t current_row, current_col;

	for(current_row = start_row; current_row < row_count; current_row++)
	{
		for(current_col = start_col; current_col < row_char_count; current_col++)
		{
			if(textSource == SMEM){	
				nextChar = *(text++);
			}
			else if(textSource == SPIMEM){ 
				nextChar = RAM::read((uint8_t*)text++);
			}
			else{
				nextChar = pgm_read_byte(text++); 
			}
			
			if(nextChar == '\0'){
				setTextIndex(char_count);
				setImageIndex(image);
				//moveCursor(current_row * 8, current_col * char_width);
				return;
			}
			else if(nextChar == '\n'){
				uint8_t space_width = row_width - (current_col * (char_width + 1));
				char_count += space_width / (char_width + 1);
				
				for(uint8_t i=0; i<space_width; i++){
					for(uint8_t charRow = 0; charRow < char_height / 8; charRow++){
						RAM::write(image + charRow * row_width, 0x00);
					}
					image++;
				}
				image += (char_height / 8 - 1) * row_width;
				current_col = row_char_count; 
				break;
			}
			else if(nextChar == '\r'){
				current_col--;
			}
			else{
				uint8_t indexOffset = ((nextChar - 0x20) / charsetCharCountH) * charsetCharCountH;
				indexOffset *= (char_height / 8 - 1);
				for(uint8_t curData = 0; curData < char_width; curData++)
				{
					for(uint8_t charRow = 0; charRow < char_height / 8; charRow++){
						RAM::write(image + charRow * row_width, RAM::read(&charsetImage[nextChar - 0x20 + indexOffset][curData + charRow * charsetWidth]));
					}
					image++;
				}
				
				uint8_t charRow;
				for(charRow = 0; charRow < 1; charRow++){
					RAM::write(image + charRow * row_width, 0x00);
				}
				image++;
			}
			
			if(nextChar != '\r')
				char_count++;
		}
		
		if(nextChar != '\n'){
			for(uint8_t i=0; i<row_remaining_space; i++)
			{
				for(uint8_t charRow = 0; charRow < char_height / 8; charRow++){
					RAM::write(image + charRow * row_width, 0x00);
				}
				image++;
			}
			image += (char_height / 8 - 1) * row_width;
		}
		start_col = 0;
	}
	
	setTextIndex(char_count);
	setImageIndex(image);
	//moveCursor((current_row - 1) * 8, current_col * char_width);
}

void TextBox::setCursor(uint8_t value){
	if(value){
		addChild(&cursor_clip);
		cursor_clip.play();
	}
	else{
		cursor_clip.gotoAndStop(0);
		removeChild(&cursor_clip);
	}
}

void TextBox::clear(){
	uint8_t* image_address = &getImage()[4];
	uint16_t col_size = (uint16_t)getImageIndex() - (uint16_t)image_address;
	RAM::writeLoop(image_address, 0, col_size);
	setTextIndex(0);
	setImageIndex(image_address);
	//moveCursor(0, 0);
}

/*void TextBox::moveCursor(uint8_t row, uint8_t col){
	cursor_clip.setX(col);
	cursor_clip.setY(row);
}*/

void TextBox::setTextIndex(uint16_t index){
	RAM::write16(&my_object->textIndex, index);
}

void TextBox::setImageIndex(uint8_t *index){
	RAM::writePtr(&my_object->imageIndex, index);
}

void TextBox::setCharset(uint8_t type){
	Charset* charset = &Manager::charset8;
	if(type == TYPE_16){
		charset = &Manager::charset16;
	}
	clear();
	RAM::writePtr(&my_object->charset, charset);
}

Charset* TextBox::getCharset(){
	return (Charset*)RAM::readPtr(&my_object->charset);
}

uint16_t TextBox::getTextIndex(){
	return RAM::read16(&my_object->textIndex);
}

uint8_t* TextBox::getImageIndex(){
	return (uint8_t*)RAM::readPtr(&my_object->imageIndex);
}






