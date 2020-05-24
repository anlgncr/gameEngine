#include "Tween.h"
Tween::tweenPool Tween::pool = {};

Tween::Tween(){}

void Tween::begin(){
	if(pool.tweens != NULL)
		return;
	
	pool.tweens = RAM::malloc(sizeof(tweenObject*) * TWEEN_MAX_SIZE);
	pool.usedTweens = RAM::malloc(sizeof(tweenObject*) * TWEEN_MAX_SIZE);
}

Tween::tweenObject* Tween::start(int16_t* var, int16_t initVal, int16_t endVal, uint32_t totalTime, uint8_t memory){	
	return init(var, initVal, endVal, totalTime, memory);
}

Tween::tweenObject* Tween::start(int16_t* var, int16_t initVal, int16_t endVal, uint32_t totalTime){	
	return init(var, initVal, endVal, totalTime, SMEM);
}

Tween::tweenObject* Tween::init(int16_t* var, int16_t initVal, int16_t endVal, uint32_t totalTime, uint8_t memory)
{	
	if((millis() + totalTime) < totalTime)//If timer register overflows
		return;
	
	if(pool.indexOfTweens >= TWEEN_MAX_SIZE)
		return;
	
	tweenObject *mTween;
	tweenObject tempTween = {};
	
	tempTween.var = var;
	tempTween.initVal = initVal;
	tempTween.endVal = endVal;
	tempTween.totalTime = totalTime;
	tempTween.initTime = millis();
	tempTween.repeatCount = 1;
	tempTween.memory = memory;
	
	mTween = findVariable(var);
	if(!mTween){
		if(pool.indexOfUsedTweens > 0){
			mTween = getUsedTweenAt(pool.indexOfUsedTweens - 1);
			pool.indexOfUsedTweens--;
		}
		else{
			mTween = RAM::malloc(sizeof(tweenObject));
		}
		setTweenAt(pool.indexOfTweens++, mTween);
	}
	RAM::writeArray(mTween, &tempTween, sizeof(tweenObject));
	return mTween;
}

Tween::tweenObject* Tween::findVariable(uint16_t* var){
	tweenObject* mTween;
	for(uint8_t i = 0; i < pool.indexOfTweens; i++){
		mTween = getTweenAt(i);
		uint16_t* tweenVar = RAM::readPtr(&mTween->var);
		if(tweenVar == var){
			return mTween;
		}
	}
	return NULL;
}

void Tween::dispose(){
	uint8_t currentIndex = 0;
	for(uint8_t i=0; i<pool.indexOfTweens; i++){
		tweenObject* mTween = getTweenAt(i);
		if(mTween != NULL){
			if(currentIndex != i){			
				setTweenAt(currentIndex, mTween);
				setTweenAt(i, NULL);
			}
			currentIndex++;
		}
	}
	pool.indexOfTweens = currentIndex;
}

void Tween::update()
{		
	for(uint8_t i=0; i< pool.indexOfTweens; i++)
	{
		tweenObject* mTween = getTweenAt(i);
		
		int16_t *var = getVariable(mTween);
		uint8_t memory = getMemory(mTween);
		uint32_t totalTime = getTotalTime(mTween);
		uint32_t elapsed_time = millis() - getInitTime(mTween);
		
		if(elapsed_time >= totalTime)
		{
			//elapsed_time = totalTime;
					
			if(memory == SPIMEM){
				setVariableVal(var, getEndVal(mTween));
			}
			else if(memory == SMEM){
				*var = getEndVal(mTween);
			}
			
			uint16_t repeatCount = getRepeatCount(mTween);
			
			if(isReversing(mTween) && repeatCount % 2 == 0){	
				int16_t temp = getInitVal(mTween);
				setInitVal(mTween, getEndVal(mTween));
				setEndVal(mTween, temp);
			}
			
			if(repeatCount > 1){
				setInitTime(mTween, millis());
				setRepeatCount(mTween, repeatCount - 1);
			}
			else if(repeatCount == 0){
				setInitTime(mTween, millis());
			}
			else{
				fPtr function = getOnFinish(mTween);
				setFinished(mTween, true);
				setTweenAt(i, NULL);
				dispose();
				setUsedTweenAt(pool.indexOfUsedTweens++, mTween);

				if(function){
					function(mTween);
				}
			}
		}
		else{
			int16_t init_value = getInitVal(mTween);
			int16_t value = getEndVal(mTween) - init_value;
			float ratio = (float)elapsed_time / (float)getTotalTime(mTween);
			
			float cal_ratio = 1;
			switch(getAnimation(mTween)){
				case LINEAR: 					cal_ratio = linear(ratio); 				break;
				case EASE_IN: 					cal_ratio = easeIn(ratio); 				break;
				case EASE_OUT: 					cal_ratio = easeOut(ratio); 			break;
				case EASE_IN_BOUNCE: 			cal_ratio = easeInBounce(ratio); 		break;
				case EASE_OUT_BOUNCE: 			cal_ratio = easeOutBounce(ratio); 		break;
				case EASE_IN_BACK: 				cal_ratio = easeInBack(ratio); 			break;
				case EASE_OUT_BACK: 			cal_ratio = easeOutBack(ratio); 		break;
				case EASE_IN_ELASTIC: 			cal_ratio = easeInElastic(ratio); 		break;
				case EASE_OUT_ELASTIC: 			cal_ratio = easeOutElastic(ratio); 		break;
				case EASE_IN_OUT: 				cal_ratio = easeInOut(ratio); 			break;
				case EASE_OUT_IN: 				cal_ratio = easeOutIn(ratio); 			break;
				case EASE_IN_OUT_ELASTIC: 		cal_ratio = easeInOutElastic(ratio); 	break;
				case EASE_OUT_IN_ELASTIC: 		cal_ratio = easeOutInElastic(ratio); 	break;
			}
			int16_t result = init_value + round(value * cal_ratio);
			if(memory == SPIMEM){
				setVariableVal(var, result);
			}
			else if(memory == SMEM){
				*var = result;
			}
		}
	}		
}

//-------------------------------------- SETTER -->

void Tween::setId(tweenObject* object, uint16_t id){
	RAM::write16(&object->id, id);
}

void Tween::setAnimation(tweenObject* object, uint8_t value){
	value = constrain(value, 0, NUMBER_OF_FUNCTION);
	RAM::write(&object->animation, value);
}

void Tween::setVariableVal(int16_t* var, int16_t value){
	RAM::write16(var, value);
}

void Tween::setFinished(tweenObject* object, uint8_t value){
	RAM::write(&object->finished, value);
}

void Tween::setReverse(tweenObject* object, uint8_t value){
	RAM::write(&object->reverse, value);
}

void Tween::setRepeatCount(tweenObject* object, uint16_t value){
	RAM::write16(&object->repeatCount, value);
}

void Tween::setInitTime(tweenObject* object,uint32_t value){
	RAM::write32(&object->initTime, value);
}

void Tween::setInitVal(tweenObject* object, int16_t value){
	RAM::write16(&object->initVal, value);
}

void Tween::setEndVal(tweenObject* object, int16_t value){
	RAM::write16(&object->endVal, value);
}

void Tween::setOnFinish(tweenObject* object, fPtr onFinish){
	RAM::writePtr(&object->onFinish, (void*)onFinish);
}

//-------------------------------------- GETTER -->

uint16_t Tween::getId(tweenObject* object){
	return RAM::read16(&object->id);
}

Tween::tweenObject* Tween::getTweenAt(uint16_t index){
	return RAM::readPtr(&pool.tweens[index]);
}

Tween::tweenObject* Tween::getUsedTweenAt(uint16_t index){
	return RAM::readPtr(&pool.usedTweens[index]);
}

void Tween::setTweenAt(uint16_t index, tweenObject* mTween){
	RAM::writePtr(&pool.tweens[index], mTween);
}

void Tween::setUsedTweenAt(uint16_t index, tweenObject* mTween){
	RAM::writePtr(&pool.usedTweens[index], mTween);
}


uint8_t Tween::getMemory(tweenObject* object){
	return RAM::read(&object->memory);
}

uint8_t Tween::getAnimation(tweenObject* object){
	return RAM::read(&object->animation);
}

Tween::fPtr Tween::getOnFinish(tweenObject* object){
	return (fPtr)RAM::readPtr(&object->onFinish);
}

uint8_t Tween::getRepeatCount(tweenObject* object){
	return RAM::read(&object->repeatCount);
}

uint8_t Tween::isReversing(tweenObject* object){
	return RAM::read(&object->reverse);
}

int16_t* Tween::getVariable(tweenObject* object){
	return RAM::readPtr(&object->var);
}

int16_t Tween::getInitVal(tweenObject* object){
	return RAM::read16(&object->initVal);
}

int16_t Tween::getEndVal(tweenObject* object){
	return RAM::read16(&object->endVal);
}

uint32_t Tween::getInitTime(tweenObject* object){
	return RAM::read32(&object->initTime);
}

uint32_t Tween::getTotalTime(tweenObject* object){
	return RAM::read32(&object->totalTime);
}

uint8_t Tween::isFinished(tweenObject* object){
	return RAM::read(&object->finished);
}


/* Methods for transition */

float Tween::linear(float ratio){
	return ratio;
}

float Tween::easeIn(float ratio){
  return ratio * ratio * ratio;
}

float Tween::easeOut(float ratio){
  float invRatio = ratio - 1;
  return invRatio * invRatio * invRatio +1; 
}

float Tween::easeInElastic(float ratio){
	if(ratio == 1 || ratio ==0)
		return ratio;
	else{
		float p = 0.3;
		float s = p/4.0;
		float invRatio = ratio - 1;
		return -1.0 * pow(2.0, 10.0 * invRatio) * sin((invRatio-s)*(2.0* 3.14)/p);
	}
}

float Tween::easeInBounce(float ratio){
	return 1.0 - easeOutBounce(1.0 - ratio);
}

float Tween::easeOutBounce(float ratio){
	float s= 7.5625;
	float p= 2.75;
	float l;
	if (ratio < (1.0/p)){
		l = s * pow(ratio, 2);
	}
	else{
		if (ratio < (2.0/p)){
			ratio -= 1.5/p;
			l = s * pow(ratio, 2) + 0.75;
		}
		else{
			if (ratio < 2.5/p){
				ratio -= 2.25/p;
				l = s * pow(ratio, 2) + 0.9375;
			}
			else{
				ratio -= 2.625/p;
				l =  s * pow(ratio, 2) + 0.984375;
			}
		}
	}
	return l;
}

float Tween::easeInBack(float ratio){
	float s = 1.70158;
	return pow(ratio, 2) * ((s + 1.0)*ratio - s);
}
		
float Tween::easeOutBack(float ratio){
	float invRatio = ratio - 1.0;            
	float s = 1.70158;
	return pow(invRatio, 2) * ((s + 1.0)*invRatio + s) + 1.0;
}

float Tween::easeOutElastic(float ratio){
	if (ratio == 0 || ratio == 1) return ratio;
	else{
		float p = 0.3;
		float s = p/4.0;                
		return pow(2.0, -10.0*ratio) * sin((ratio-s)*(2.0*3.14)/p) + 1;                
	}            
}      	

float Tween::easeInOut(float ratio){
	return (ratio < 0.5)? easeIn(ratio) : easeOut(ratio);
} 

float Tween::easeOutIn(float ratio){
	return (ratio < 0.5)? easeOut(ratio) : easeIn(ratio);
} 

float Tween::easeInOutElastic(float ratio){
	return (ratio < 0.5)? easeInElastic(ratio) : easeOutElastic(ratio);
} 

float Tween::easeOutInElastic(float ratio){
	return (ratio < 0.5)? easeOutElastic(ratio) : easeInElastic(ratio);	
}








