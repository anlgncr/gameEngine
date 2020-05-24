#ifndef Tween_h
#define Tween_h
#include "arduino.h"
#include "RAM.h"

#define TWEEN_MAX_SIZE 100

#define LINEAR 0
#define EASE_IN 1
#define EASE_OUT 2
#define EASE_IN_BOUNCE 3
#define EASE_OUT_BOUNCE 4
#define EASE_IN_BACK 5
#define EASE_OUT_BACK 6
#define EASE_IN_ELASTIC 7 
#define EASE_OUT_ELASTIC 8
#define EASE_IN_OUT 9
#define EASE_OUT_IN 10
#define EASE_OUT_IN_ELASTIC 11 
#define EASE_IN_OUT_ELASTIC 12
#define NUMBER_OF_FUNCTION 13

class Tween{
	public:
		Tween();
		
		struct tweenObject;
		
		typedef void (*fPtr)(tweenObject*);
		struct tweenObject{
			uint16_t id;
			int16_t* var;
			int16_t initVal;
			int16_t endVal;
			uint32_t initTime;
			uint32_t totalTime;
			uint16_t repeatCount;
			uint8_t animation;
			uint8_t finished;
			uint8_t memory;
			uint8_t reverse;
			fPtr onFinish;
		};
		
		struct tweenPool{
			tweenObject **tweens;
			tweenObject **usedTweens;
			uint16_t indexOfTweens;
			uint16_t indexOfUsedTweens;
		};
		static tweenPool pool;
		
		static void begin();
		static void update();
		static tweenObject* start(int16_t* var, int16_t initVal, int16_t endVal, uint32_t totalTime);
		static tweenObject* start(int16_t* var, int16_t initVal, int16_t endVal, uint32_t totalTime, uint8_t memory);
		
		
		static uint16_t getId(tweenObject*);
		static int16_t* getVariable(tweenObject*);
		static int16_t getInitVal(tweenObject*);
		static int16_t getEndVal(tweenObject*);
		static uint32_t getInitTime(tweenObject*);
		static uint32_t getTotalTime(tweenObject*);
		static uint8_t getRepeatCount(tweenObject*);
		static uint8_t isReversing(tweenObject*);
		static uint8_t getAnimation(tweenObject*);
		static uint8_t getMemory(tweenObject*);
		static uint8_t isFinished(tweenObject*);

		static void setId(tweenObject*, uint16_t);
		static void setMemory(tweenObject*, uint8_t);
		static void setAnimation(tweenObject*, uint8_t);
		static void setOnFinish(tweenObject*, fPtr);
		static void setRepeatCount(tweenObject*, uint16_t);
		static void setReverse(tweenObject*, uint8_t);
	
	private:
		
		static tweenObject* init(int16_t* var, int16_t initVal, int16_t endVal, uint32_t totalTime, uint8_t memory);
		static tweenObject* findVariable(uint16_t*);
		static void dispose();
		static void setTweenAt(uint16_t index, tweenObject* mTween);
		static void setUsedTweenAt(uint16_t index, tweenObject* mTween);
		static void setIndexOfTweens(uint16_t index);
		static void setIndexOfUsedTweens(uint16_t index);
		
		static tweenObject* getTweenAt(uint16_t index);
		static tweenObject* getUsedTweenAt(uint16_t index);

		static void setFinished(tweenObject*, uint8_t);
		static void setVariable(int16_t*);
		static void setInitVal(tweenObject*, int16_t);
		static void setEndVal(tweenObject*, int16_t);
		static void setTotalTime(tweenObject*, uint32_t);
		static void setVariableVal(int16_t*, int16_t);
		static void setInitTime(tweenObject*, uint32_t);

		static fPtr getOnFinish(tweenObject*);

		typedef float(Tween::*tweenFunction)(float);
		
		static float linear(float); 			
		static float easeIn(float); 			
		static float easeOut(float); 		
		static float easeInBounce(float); 	
		static float easeOutBounce(float); 	
		static float easeInBack(float); 		
		static float easeOutBack(float); 	
		static float easeInElastic(float); 	
		static float easeOutElastic(float); 	
		static float easeInOut(float); 		
		static float easeOutIn(float); 		
		static float easeOutInElastic(float);
		static float easeInOutElastic(float);
};
#endif