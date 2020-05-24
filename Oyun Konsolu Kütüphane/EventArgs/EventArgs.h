#ifndef EventArgs_h
#define EventArgs_h
#include "arduino.h"

class DisplayObject;
class EventArgs{
	public:
		EventArgs();
		
		DisplayObject* getSender();
		DisplayObject* getReceiver();
		uint16_t getData();
		void* getObject();
		uint8_t getEventName();
		
		void setSender(DisplayObject*);
		void setReceiver(DisplayObject*);
		void setData(uint16_t);
		void setObject(void*);
		void setEventName(uint8_t);
			
	private:
		struct object{
			DisplayObject* sender;
			DisplayObject* receiver;
			uint8_t eventName;
			uint16_t data;
			void* object;
		};
		object eventObject;
};

#endif
	
	