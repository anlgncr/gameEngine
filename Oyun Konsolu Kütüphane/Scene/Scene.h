#ifndef Scene_h
#define Scene_h
#include "arduino.h"
#include "DisplayObject.h"
#include "RAM.h"

#define USE_SPIRAM_SCREEN_BUFFER

#ifdef USE_SPIRAM_SCREEN_BUFFER
	#include "ScreenSPIRAM.h"
#else
	#include "Screen.h"
#endif

class Manager;
class Scene : public DisplayObject{
	public:
		Scene(uint8_t childSize);
				
		void onUpdate();
		void updateChild(DisplayObject* child);
		
		void setInverse(uint8_t value);
		void adjustBrightness();
		
		void onButtonDown(uint8_t button);
		
		void sleep();
		void wakeUp();
		bool isSleeping();
		
	private:
		#ifdef USE_SPIRAM_SCREEN_BUFFER
			ScreenSPIRAM myScreen;
		#else
			Screen myScreen;
		#endif
		
		uint8_t inverse = true;
		uint8_t sleeping = false;
		uint8_t autoBrightness = true;
		
};

#endif