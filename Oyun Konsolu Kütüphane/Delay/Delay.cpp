#include "Delay.h"
Delay::delayPool Delay::pool = {};

Delay::Delay(){}

void Delay::begin(){
	if(pool.delays != NULL)
		return;

	pool.delays = RAM::malloc(sizeof(delayObject*) * DELAY_MAX_SIZE);
	pool.usedDelays = RAM::malloc(sizeof(delayObject*) * DELAY_MAX_SIZE);
}

Delay::delayObject* Delay::start(fPtr function, uint32_t totalTime)
{	
	if((millis() + totalTime) < totalTime)//If timer register overflows
		return;
	
	if(pool.indexOfDelays >= DELAY_MAX_SIZE)
		return;
	
	delayObject *mDelay;
	delayObject tempDelay = {};
	
	tempDelay.totalTime = totalTime;
	tempDelay.initTime = millis();
	tempDelay.repeatCount = 1;
	tempDelay.onFinish = function; 
	
	if(pool.indexOfUsedDelays > 0){
		mDelay = getUsedDelayAt(pool.indexOfUsedDelays - 1);
		pool.indexOfUsedDelays--;
	}
	else{
		mDelay = RAM::malloc(sizeof(delayObject));
	}
	
	setDelayAt(pool.indexOfDelays++, mDelay);
	RAM::writeArray(mDelay, &tempDelay, sizeof(delayObject));
	return mDelay;
}

void Delay::dispose(){
	uint8_t currentIndex = 0;
	for(uint8_t i=0; i<pool.indexOfDelays; i++){
		delayObject* mDelay = getDelayAt(i);
		if(mDelay != NULL){
			if(currentIndex != i){			
				setDelayAt(currentIndex, mDelay);
				setDelayAt(i, NULL);
			}
			currentIndex++;
		}
	}
	pool.indexOfDelays = currentIndex;
}

void Delay::update()
{		
	for(uint8_t i=0; i< pool.indexOfDelays; i++)
	{
		delayObject* mDelay = getDelayAt(i);
		uint32_t elapsed_time = millis() - getInitTime(mDelay);
		
		if(elapsed_time >= getTotalTime(mDelay))
		{
			uint16_t repeatCount = getRepeatCount(mDelay);
			setRepeatCount(mDelay, --repeatCount);
			fPtr function = getOnFinish(mDelay);
			function(mDelay);
			
			if(repeatCount == 0){
				setFinished(mDelay, true);
				setDelayAt(i, NULL);
				dispose();
				setUsedDelayAt(pool.indexOfUsedDelays++, mDelay);
			}
			else{
				setInitTime(mDelay, millis());
			}
		}
	}		
}

//-------------------------------------- SETTER -->
void Delay::setFinished(delayObject* object, uint8_t value){
	RAM::write(&object->finished, value);
}

void Delay::setRepeatCount(delayObject* object, uint16_t value){
	RAM::write16(&object->repeatCount, value);
}

void Delay::setInitTime(delayObject* object,uint32_t value){
	RAM::write32(&object->initTime, value);
}

void Delay::setOnFinish(delayObject* object, fPtr onFinish){
	RAM::writePtr(&object->onFinish, (void*)onFinish);
}

//-------------------------------------- GETTER -->

Delay::delayObject* Delay::getDelayAt(uint16_t index){
	return RAM::readPtr(&pool.delays[index]);
}

Delay::delayObject* Delay::getUsedDelayAt(uint16_t index){
	return RAM::readPtr(&pool.usedDelays[index]);
}

void Delay::setDelayAt(uint16_t index, delayObject* mDelay){
	RAM::writePtr(&pool.delays[index], mDelay);
}

void Delay::setUsedDelayAt(uint16_t index, delayObject* mDelay){
	RAM::writePtr(&pool.usedDelays[index], mDelay);
}


Delay::fPtr Delay::getOnFinish(delayObject* object){
	return (fPtr)RAM::readPtr(&object->onFinish);
}

uint8_t Delay::getRepeatCount(delayObject* object){
	return RAM::read(&object->repeatCount);
}

uint32_t Delay::getInitTime(delayObject* object){
	return RAM::read32(&object->initTime);
}

uint32_t Delay::getTotalTime(delayObject* object){
	return RAM::read32(&object->totalTime);
}

uint8_t Delay::isFinished(delayObject* object){
	return RAM::read(&object->finished);
}









