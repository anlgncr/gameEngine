#include "EventArgs.h"
#include "DisplayObject.h"

EventArgs::EventArgs(){
	
}

void EventArgs::setSender(DisplayObject* sender){
	eventObject.sender = sender;
}

void EventArgs::setReceiver(DisplayObject* receiver){
	eventObject.receiver = receiver;
}

void EventArgs::setData(uint16_t data){
	eventObject.data = data;
}

void EventArgs::setEventName(uint8_t eventName){
	eventObject.eventName = eventName;
}

void EventArgs::setObject(void* object){
	eventObject.object = object;
}

uint8_t EventArgs::getEventName(){
	return eventObject.eventName;
}

DisplayObject* EventArgs::getSender(){
	return eventObject.sender;
}

DisplayObject* EventArgs::getReceiver(){
	return eventObject.receiver;
}

uint16_t EventArgs::getData(){
	return eventObject.data;
}

void* EventArgs::getObject(){
	return eventObject.object;
}