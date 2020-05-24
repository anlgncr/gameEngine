#ifndef Delay_h
#define Delay_h
#include "arduino.h"
#include "RAM.h"

#define DELAY_MAX_SIZE 100

class Delay{
	public:
		Delay();
		struct delayObject;
		
		typedef void (*fPtr)(delayObject*);
		struct delayObject{
			uint8_t id;
			uint32_t initTime;
			uint32_t totalTime;
			uint16_t repeatCount;
			uint8_t finished;
			fPtr onFinish;
		};
		
		struct delayPool{
			delayObject **delays;
			delayObject **usedDelays;
			uint16_t indexOfDelays;
			uint16_t indexOfUsedDelays;
		};
		static delayPool pool;
		
		static void begin();
		static void update();
		static delayObject* start(fPtr function, uint32_t totalTime);
		
		static uint32_t getInitTime(delayObject*);
		static uint32_t getTotalTime(delayObject*);
		static uint8_t getRepeatCount(delayObject*);
		static uint8_t isFinished(delayObject*);

		static void setOnFinish(delayObject*, fPtr);
		static void setRepeatCount(delayObject*, uint16_t);
	
	private:
		static void dispose();
		static void setDelayAt(uint16_t index, delayObject* mDelay);
		static void setUsedDelayAt(uint16_t index, delayObject* mDelay);
		static void setIndexOfDelays(uint16_t index);
		static void setIndexOfUsedDelays(uint16_t index);
		
		static delayObject* getDelayAt(uint16_t index);
		static delayObject* getUsedDelayAt(uint16_t index);

		static void setFinished(delayObject*, uint8_t);
		static void setInitTime(delayObject*, uint32_t);
		static fPtr getOnFinish(delayObject*);
};
#endif