#include "MovieClip.h"
#include "Manager.h"

MovieClip::MovieClip(uint8_t childSize) : DisplayObject(childSize){}
MovieClip::MovieClip() : DisplayObject(0){}

bool MovieClip::createFromProgmem(uint8_t **images, uint8_t frameCount, uint8_t FPS){
	if(frameCount < 1)
		return;
	
	setMemory(PGMEM);
	return createMovieClip(images, frameCount, FPS);
}

bool MovieClip::createFromFile(char* fileName){
	FileHandler::FILE newFile;
	bool fileExists = Manager::fileHandler.getFile(fileName, &newFile);
	if(fileExists && newFile.size > 0)
	{		
		uint8_t resolution[11];
		Manager::fileHandler.fileToSram(&newFile, resolution, 11);
		
		uint16_t width = Convert::toUInt16(resolution + 3); // Atlas'ın genişliği
		uint16_t height = Convert::toUInt16(resolution + 5); // Atlas'ın yüksekliği
		uint16_t widthPart = Convert::toUInt16(resolution + 7); // Görüntü parçasının genişliği
		uint16_t heightPart = Convert::toUInt16(resolution + 9); // Görüntü parçasının yüksekliği

		if(height != 0 && width != 0 && heightPart != 0 && widthPart != 0 && widthPart <= width && heightPart <= height)
		{
			uint16_t rowCount = height / 8;
			rowCount += (height % 8 == 0)? 0 : 1;
			
			uint8_t* image = (uint8_t*)RAM::mallocStack(newFile.size); // Görüntü atlası için geçici buffer
			Manager::fileHandler.fileToSpiRam(&newFile, image, newFile.size); // Atlası buffer'a kopyala
			
			uint16_t rowCountPart = heightPart / 8;
			rowCountPart += (heightPart % 8 == 0)? 0 : 1;
			
			uint16_t imageCountH = width / widthPart; // Yatay görüntü sayısı
			uint16_t imageCountV = height / heightPart; // Dikey görüntü sayısı
			uint8_t frameCount = imageCountH * imageCountV; // Toplam görüntü sayısı
			uint8_t FPS = RAM::read(image + 11);
			uint8_t maskType = RAM::read(image + 12);
			
			if(maskType == IMAGE_SELF_MASK)
				frameCount /= 2;
				
			uint8_t **images = (uint8_t**)RAM::malloc(frameCount * sizeof(uint8_t*)); // Görüntülerin saklandığı dizi			
			uint8_t currentFrame = 0;
			for(uint16_t v = 0; v < imageCountV; v++) // Dikey görüntü sayısı kadar
			{
				for(uint16_t h = 0; h < imageCountH; h++) // Yatay görüntü sayısı kadar
				{
					uint8_t* imagePart; // RAM' de görüntü için yer ayır
					uint16_t index = 4; // Görüntü parçasının genişlik ve yüksekliği için
					
					if(maskType == IMAGE_SELF_MASK){
						currentFrame++;
						//Eğer sonda fazladan bir kare varsa döngüden çık
						//Maskeli klipler çift halinde olmalıdır.
						
						if(currentFrame > frameCount * 2) 
							break;
						
						if(currentFrame % 2 == 0){
							imagePart = (uint8_t*)RAM::read16(&images[currentFrame / 2 - 1]);
							index += rowCountPart * widthPart;
						}
						else{
							imagePart = (uint8_t*)RAM::malloc((2 * widthPart * rowCountPart) + 4);
							RAM::write16(imagePart, widthPart);
							RAM::write16(imagePart + 2, heightPart);
							RAM::write16(&images[currentFrame / 2], imagePart); // Görüntü parçasını diziye kaydet
						}
					}
					else{
						imagePart = (uint8_t*)RAM::malloc((widthPart * rowCountPart) + 4);
						RAM::write16(imagePart, widthPart);
						RAM::write16(imagePart + 2, heightPart);
						RAM::write16(&images[v * imageCountH + h], imagePart); // Görüntü parçasını diziye kaydet
					}
					
					// Görüntü atlasından sıradaki görüntü parçasını kes
					for(uint16_t j=0; j < rowCountPart; j++)
					{
						for(uint16_t i=0; i < widthPart; i++)
						{
							uint16_t currentRow = width * (rowCountPart * v + j);
							uint8_t data = RAM::read(&image[currentRow + widthPart * h + i] + 13);
							RAM::write(&imagePart[index++], data);
						}
					}
				}
			}
			setMaskType(maskType);
			RAM::popStack(); // Atlas için ayrılan hafızayı serbest bırak
			setMemory(SPIMEM); // Görüntünün kaynağı SPI RAM
			createMovieClip(images, frameCount, FPS); // MovieClip'i başlat
			return true;
		}
	}
	
	uint8_t** images = (uint8_t**)malloc(sizeof(uint8_t*));
	images[0] = (uint8_t*)noImage;
	setMemory(PGMEM);
	setMaskType(IMAGE_AUTO_MASK);
	createMovieClip(images, 1, 1);
	return false;
}

void MovieClip::createInstanceOf(MovieClip* movie){
	if(movie == NULL)
		return;
	
	createMovieClip(movie->getImages(), movie->getFrameCount(), movie->getFPS());
	setMemory(movie->getMemory());
}

bool MovieClip::createMovieClip(uint8_t **images, uint8_t frameCount, uint8_t FPS){
	my_object = (object*)RAM::malloc(sizeof(object));
	if(!my_object)
		return false;
	
	object new_object = {};
	new_object.frameChangeTimes = (uint32_t*)RAM::malloc(frameCount * sizeof(uint32_t));
	if(!new_object.frameChangeTimes)
		return false;
	
	new_object.images = images;
	new_object.frameCount = frameCount;
	new_object.FPS = FPS;
	copyObject(&new_object, my_object);		

	setImage(getImageAt(0));
	RAM::write(&my_object->lastFrame, frameCount - 1);
	return true;
}

void MovieClip::onUpdate(){
	if(isPlaying() && getFPS() != 0){
		uint8_t currentFrame = getCurrentFrame();
		uint32_t changeTime = getFrameChangeTime(currentFrame);
		
		if(millis() > changeTime){
			if(currentFrame < (getLastFrame())){
				currentFrame++;				
				setImage(getImageAt(currentFrame));
				setCurrentFrame(currentFrame);
			}
			else{
				uint8_t repeatCount = getRepeatCount();
				if(repeatCount == 0){
					gotoAndPlay(getFirstFrame());
				}
				else if(repeatCount == 1){
					gotoAndStop(getFirstFrame());
				}
				else{
					setRepeatCount(repeatCount - 1);
					gotoAndPlay(getFirstFrame());
				}
			}
		}
	}
}

void MovieClip::playMovie(){
	if(isPlaying() && my_object != NULL)
		return;
	
	uint8_t FPS = getFPS();
	if(FPS == 0)
		return;
	
	uint8_t currentFrame = getCurrentFrame();
	uint8_t remainingFrameCount = getLastFrame() + 1 - currentFrame;
	uint32_t add_time = millis() - getPassedTime(); // The time that last frame has been shown
	
	uint32_t frame_change_time; 
	uint32_t *frameChangeTimes = (uint32_t*)RAM::readPtr(&my_object->frameChangeTimes);
	for(uint8_t i = 0; i < remainingFrameCount; i++){
		frame_change_time = (i + 1) * (1000 / FPS) + add_time;
		RAM::write32(frameChangeTimes + i + currentFrame, frame_change_time);
	}
	
	setPassedTime(0);
	setPlaying(true);
}

void MovieClip::play(){
	setRepeatCount(0);
	playMovie();
}

void MovieClip::playFor(uint8_t count){
	setRepeatCount(count);
	playMovie();
}

void MovieClip::playOnce(){
	setRepeatCount(1);
	playMovie();
}

void MovieClip::stop(){
	if(!isPlaying()){
		return;
	}
	
	uint8_t FPS = getFPS();
	
	int32_t remainingTime = getFrameChangeTime(getCurrentFrame()) - millis();
	if(remainingTime <= 0 || FPS == 0){
		setPassedTime(0);
		setPlaying(false);
		return;
	}
	
	setPassedTime(1000 / FPS - remainingTime);
	setPlaying(false);
}

void MovieClip::gotoAndPlay(uint8_t frame){
	if(frame > getFrameCount())
		return;
	
	setPassedTime(0);
	setCurrentFrame(frame);
	setPlaying(false);
	setImage(getImageAt(frame));
	playMovie();
}

void MovieClip::gotoAndStop(uint8_t frame){
	if(frame > getFrameCount())
		return;
	
	setPassedTime(0);
	setCurrentFrame(frame);
	setPlaying(false);
	setImage(getImageAt(frame));
}

void MovieClip::nextFrame(){
	uint8_t currentFrame = getCurrentFrame();
	uint8_t frameCount = getFrameCount();
	
	if(currentFrame >= frameCount - 1){
		setCurrentFrame(0);
		setImage(getImageAt(0));
	}
	else{
		setCurrentFrame(currentFrame + 1);
		setImage(getImageAt(currentFrame + 1));
	}
	
}

void MovieClip::prevFrame(){
	uint8_t currentFrame = getCurrentFrame();
	uint8_t frameCount = getFrameCount();
	
	if(currentFrame <= 0){
		setCurrentFrame(frameCount - 1);
		setImage(getImageAt(frameCount - 1));
	}
	else{
		setCurrentFrame(currentFrame - 1);
		setImage(getImageAt(currentFrame - 1));
	}
}

void MovieClip::setFirstAndLastFrame(uint8_t first, uint8_t last){
	if(last >= getFrameCount() || last <= first)
		return;
		
	RAM::write(&my_object->firstFrame, first);
	RAM::write(&my_object->lastFrame, last);
}


uint8_t MovieClip::getLastFrame(){
	return RAM::read(&my_object->lastFrame);
}

uint8_t MovieClip::getFirstFrame(){
	return RAM::read(&my_object->firstFrame);
}

void MovieClip::setCurrentFrame(uint8_t value){
	if(value >= getFrameCount())
		return;
	
	RAM::write(&my_object->currentFrame, value);
}

void MovieClip::setPassedTime(uint32_t time){
	RAM::write32(&my_object->passedTime, time);
}

void MovieClip::setPlaying(uint8_t value){
	RAM::write(&my_object->isPlaying, value);
}

void MovieClip::setRepeatCount(uint8_t value){
	RAM::write(&my_object->repeatCount, value);
}

void MovieClip::setFPS(uint8_t value){
	RAM::write(&my_object->FPS, value);
	if(isPlaying()){
		setPlaying(false);
		playMovie();
	}
}

MovieClip::object* MovieClip::getMovieClipObject(){
	return my_object;
}

uint8_t MovieClip::isPlaying(){
	return RAM::read(&my_object->isPlaying);
}

uint8_t MovieClip::getCurrentFrame(){
	return RAM::read(&my_object->currentFrame);
}

uint32_t MovieClip::getFrameChangeTime(uint8_t frame){
	uint32_t *frameChangeTime = (uint32_t*)RAM::readPtr(&my_object->frameChangeTimes);
	return RAM::read32(&frameChangeTime[frame]);
}

uint8_t MovieClip::getFPS(){
	return RAM::read(&my_object->FPS);
}

uint8_t MovieClip::getFrameCount(){
	return RAM::read(&my_object->frameCount);
}

uint8_t MovieClip::getRepeatCount(){
	return RAM::read(&my_object->repeatCount);
}

uint32_t MovieClip::getPassedTime(){
	return RAM::read32(&my_object->passedTime);
}

uint8_t** MovieClip::getImages(){
	return (uint8_t**)RAM::readPtr(&my_object->images);
}

uint8_t* MovieClip::getImageAt(uint8_t index){
	uint8_t memory = getMemory();
	
	uint8_t** images = (uint8_t**)RAM::readPtr(&my_object->images);
	if(memory == SPIMEM){
		return (uint8_t*)RAM::readPtr(&images[index]);
	}
	else{
		return images[index];
	}
}

void MovieClip::writeInfo(){
	
}

void MovieClip::copyObject(object* obj1, object* obj2){ //obj1 is to be copied
	RAM::writeArray(obj2, obj1, sizeof(object));
}

void MovieClip::loadObject(object* obj1, object* obj2){
	RAM::readArray(obj2, obj1, sizeof(object));
}