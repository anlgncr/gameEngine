#include "DisplayObject.h"
#include "Manager.h"
#include "Tween.h"

DisplayObject::DisplayObject(uint8_t childSize){
	my_object = (object*)RAM::malloc(sizeof(object));
	if(!my_object)
		return;
	
	object new_object = {};
	
	if(childSize > 0){
		new_object.children = (DisplayObject**)RAM::malloc(sizeof(DisplayObject*) * childSize);
		if(!new_object.children)
			return;
	}
	
	new_object.events = (eventObject**)RAM::malloc(sizeof(eventObject*) * 10); // Max 10 listener
	new_object.eventSize = 10;
	new_object.childSize = childSize;
	new_object.visibility = true;
	new_object.memory = PGMEM;

	copyObject(&new_object, my_object);
}

void DisplayObject::copyObject(object* obj1, object* obj2){ //obj1 is to be copied
	RAM::writeArray(obj2, obj1, sizeof(object));
}

void DisplayObject::loadObject(object* obj1, object* obj2){
	RAM::readArray(obj2, obj1, sizeof(object));
}

void DisplayObject::moveX(int16_t pixel){
	float ratio = (float)Manager::getProcessTime() / (float)1000;
	setX(getX() + round(ratio * pixel));
}

void DisplayObject::moveY(int16_t pixel){
	float ratio = (float)Manager::getProcessTime() / (float)1000;
	setX(getY() + round(ratio * pixel));
}

void DisplayObject::moveTo(int16_t x, int16_t y, uint32_t time){
	moveTo(x, y, time, LINEAR);
}

void DisplayObject::moveTo(int16_t x, int16_t y, uint32_t time, uint8_t animation){
	Tween::tweenObject* tweenX = Tween::start(getXRef(), getX(), x, time, SPIMEM);
	Tween::tweenObject* tweenY = Tween::start(getYRef(), getY(), y, time, SPIMEM);
	Tween::setAnimation(tweenX, animation);
	Tween::setAnimation(tweenY, animation);
}

uint8_t DisplayObject::hitTest(DisplayObject* target){
	if(getCollider() == COLLIDER_RECTANGLE)
	{	
		int16_t x = getGlobalX();
		int16_t targetX = target->getGlobalX();
		
		if(x >=  (int16_t)(targetX + target->getWidth())){
			return false;
		}
		if((int16_t)(x + getWidth()) <= targetX){
			return false;
		}
		
		int16_t y = getGlobalY();
		int16_t targetY = target->getGlobalY();
		
		if(y >= (int16_t)(targetY + target->getHeight())){
			return false;
		}
		if((int16_t)(y + getHeight()) <= targetY){
			return false;
		}
		return true;
	}
	else{
		if(distanceTo(target) <= getRadius() + target->getRadius())
			return true;
		
		return false;
	}
}

uint16_t DisplayObject::getRadius(){
	uint16_t width = getWidth();
	uint16_t height = getHeight();
	uint16_t radius;
	
	if(width > height)
		radius = width / 2;
	else
		radius = height / 2;
	
	return radius;
}

uint16_t DisplayObject::distanceTo(DisplayObject* target){
	int16_t halfWidth = getWidth() / 2;
	int16_t halfHeight = getHeight() / 2;
	int16_t centerX = getGlobalX() + halfWidth;
	int16_t centerY = getGlobalY() + halfHeight;
	
	int16_t tHalfWidth = target->getWidth() / 2;
	int16_t tHalfHeight = target->getHeight() / 2;
	int16_t targetCenterX = target->getGlobalX() + tHalfWidth;
	int16_t targetCenterY = target->getGlobalY() + tHalfHeight;
	
	int16_t distanceX = centerX - targetCenterX;
	int16_t distanceY = centerY - targetCenterY;
	
	return sqrt(distanceX * distanceX + distanceY * distanceY);
}

void DisplayObject::setChildIndex(DisplayObject* child, uint8_t targetIndex){
	uint8_t childIndex = child->getIndex();
	if(!contains(child) || targetIndex >= getChildCount() || targetIndex == childIndex)
		return;
	
	uint8_t childCount = getChildCount();
	DisplayObject* children = RAM::mallocStack(childCount * sizeof(DisplayObject*));
	
	for(uint8_t i=0; i<childCount; i++){
		DisplayObject* child = getChildAt(i);
		if(child){
			RAM::write16(&children[child->getIndex()], child);
		}
	}
	
	if(childIndex > targetIndex){
		for(uint8_t i=0; i<childIndex - targetIndex; i++){
			DisplayObject* object = RAM::read16(&children[childIndex - i - 1]);
			if(object)
				object->setIndex(childIndex - i);
		}
	}
	else{
		for(uint8_t i=childIndex; i<targetIndex; i++){
			DisplayObject* object = RAM::read16(&children[i + 1]);
			if(object)
				object->setIndex(i);
		}	
	}
	child->setIndex(targetIndex);
	setChildIndexChanged(true);
	RAM::popStack();
}

bool DisplayObject::contains(DisplayObject* child){
	if(child == NULL)
		return false;
	
	uint8_t childCount = getChildCount();
	for(uint8_t i=0; i<childCount; i++){
		DisplayObject *current_displayObject = getChildAt(i);
		if(current_displayObject == child){
			return true;
		}
	}	
	return false;
}

void DisplayObject::addChild(DisplayObject* child){
	if(child == NULL || child == this || child->getParent() == this || child->contains(this) || contains(child))
		return;
		
	uint8_t childCount = getChildCount();
	if(childCount >= getChildSize())
		return;
	
	DisplayObject* parent = child->getParent();
	if(parent){
		parent->removeChild(child);
		child->setX(child->getGlobalX() - getGlobalX());
		child->setY(child->getGlobalY() - getGlobalY());
	}
	
	setChildAt(childCount, child);
	setChildCount(childCount + 1);
	child->setIndex(childCount);
	child->setParent(this);
	child->onAdded();
}

void DisplayObject::removeChild(DisplayObject* child){
	if(!contains(child))
		return;
	
	uint8_t childCount = getChildCount();
	
	for(uint8_t i=0; i<childCount; i++){
		if(getChildAt(i) == child){
			setChildAt(i, NULL);
			setChildRemoved(true);
			
			if(getParent() != NULL){
				getParent()->addChild(child); //add child to my parent. Bir üst parent'a ekle
			}
			else{
				child->setParent(NULL);
				child->onRemoved();
			}
			break;
		}
	}
}

void DisplayObject::remove(){
	DisplayObject *parent = getParent();
	if(parent == NULL){
		return;
	}
	
	uint8_t childCount = parent->getChildCount();
	
	for(uint8_t i=0; i<childCount; i++){
		if(parent->getChildAt(i) == this){
			parent->setChildAt(i, NULL);
			parent->setChildRemoved(true);
			setParent(NULL);
			onRemoved();
			break;
		}
	}
}

void DisplayObject::leaveParent(){
	DisplayObject *parent = getParent();
	if(parent != NULL){
		parent->removeChild(this);
	}
}

void DisplayObject::disposeChild(){
	uint8_t childCount = getChildCount();
	uint8_t currentIndex = 0;
	for(uint8_t i=0; i<childCount; i++){
		DisplayObject *current_displayObject = getChildAt(i);
		if(current_displayObject != NULL){
			if(currentIndex != i){
				setChildAt(currentIndex, current_displayObject);
				current_displayObject->setIndex(currentIndex);
				setChildAt(i, NULL);
			}
			currentIndex++;
		}
	}
	setChildCount(currentIndex);
}

void DisplayObject::dispatchEventAll(uint8_t event){
	EventArgs e;
	e.setSender(this);
	e.setEventName(event);
	dispatchEventAll(&e);
}

void DisplayObject::dispatchEventAll(EventArgs* e){
	updateEvent(e);
	uint8_t childCount = getChildCount();
	
	if(childCount > 0){
		applyChildChanges();
		for(uint8_t i=0; i<childCount; i++){
			DisplayObject *object = getChildAt(i);
			
			if(object){
				object->dispatchEventAll(e);
			}
		}	
	}
}

void DisplayObject::applyChildChanges(){
	if(getChildIndexChanged()){
		uint8_t childCount = getChildCount();
		DisplayObject* children[childCount] = {};
		
		for(uint8_t i=0; i<childCount; i++){
			DisplayObject* child = getChildAt(i);
			if(child)
				children[child->getIndex()] = child;
		}
		
		for(uint8_t i=0; i<childCount; i++){
			setChildAt(i, children[i]);
		}
		setChildIndexChanged(false);
	}
	
	if(getChildRemoved()){
		disposeChild();
		setChildRemoved(false);
	}
}

void DisplayObject::dispatchEvent(uint8_t event){
	EventArgs e;
	e.setSender(this);
	e.setEventName(event);
	dispatchEvent(&e);
}

void DisplayObject::dispatchEvent(EventArgs* e){
	DisplayObject *parent = getParent();
	if(parent){
		parent->dispatchEvent(e); //Bubbling ..oooOOO
	}
	updateEvent(e); //Catching \☻/
}

void DisplayObject::updateEvent(EventArgs* e){
	uint8_t eventCount = getEventCount();
	uint16_t info = e->getData();
	uint8_t event = e->getEventName();
	
	switch(event){
		case BUTTON_DOWN: onButtonDown(info); break;
		case BUTTON_UP: onButtonUp(info); break;
		case BUTTON_LONG_DOWN: onButtonLongDown(info); break;
		default : onSpecificEvent(event);
	}

	for(uint8_t i=0; i<eventCount; i++){
		eventObject *event_object = getEventAt(i);
		uint8_t my_event = getEvent(event_object);
		if(my_event == event){
			eventFunction function = getEventFunction(event_object);
			if(function){
				e->setReceiver(this);
				function(e);
			}
		}
	}
}

void DisplayObject::addEventListener(uint8_t event, eventFunction function){
	uint8_t eventCount = getEventCount();
	uint8_t size = getEventSize();
	
	if(eventCount >= size)
		return;	
	
	for(uint8_t i=0; i<eventCount; i++){
		eventObject *my_event = getEventAt(i);
		eventFunction my_function = getEventFunction(my_event);
		if(my_function == function)
			return;
	}
	
	eventObject *new_event = (eventObject*)RAM::malloc(sizeof(eventObject));
	if(new_event){
		setEventFunction(new_event, function);
		setEvent(new_event, event);
		setEventAt(eventCount, new_event);
		setEventCount(eventCount + 1);
	}
}

void DisplayObject::removeEventListener(uint8_t event){
	uint8_t eventCount = getEventCount();
	
	for(uint8_t i=0; i<eventCount; i++){
		eventObject *event_object = getEventAt(i);
		uint8_t my_event = getEvent(event_object);
		if(my_event == event){
			setEventAt(i, NULL);
			disposeEvent();
			break;
		}
	}
}

void DisplayObject::disposeEvent(){
	uint8_t currentIndex = 0;
	uint8_t eventCount = getEventCount();
	
	for(uint8_t i=0; i<eventCount; i++){
		eventObject *event_object = getEventAt(i);
		if(event_object != NULL){
			if(currentIndex != i){
				setEventAt(currentIndex, event_object);
				setEventAt(i, NULL);
			}
			currentIndex++;
		}
	}
	setEventCount(currentIndex);
}

DisplayObject::eventFunction DisplayObject::getEventFunction(eventObject* event_object){
	return (eventFunction)RAM::readPtr(&event_object->function);
}

void DisplayObject::setEventFunction(eventObject* event_object, eventFunction function){
	RAM::writePtr(&event_object->function, (void*)function);
}

void DisplayObject::setEvent(eventObject* event_object, uint8_t event){
	RAM::write(&event_object->event, event);
}

uint8_t DisplayObject::getEvent(eventObject* event_object){
	return RAM::read(&event_object->event);
}

uint8_t DisplayObject::getEventCount(){
	return RAM::read(&my_object->eventCount); 
}

uint8_t DisplayObject::getEventSize(){
	return RAM::read(&my_object->eventSize);
}

void DisplayObject::setEventCount(uint8_t value){
	RAM::write(&my_object->eventCount, value);
}

void DisplayObject::setEventSize(uint8_t value){
	RAM::write(&my_object->eventSize, value);
}

void DisplayObject::setEventAt(uint8_t index, eventObject *event){
	eventObject** events = (eventObject**)RAM::readPtr(&my_object->events);
	RAM::writePtr(&events[index], event);
}

DisplayObject::eventObject* DisplayObject::getEventAt(uint8_t index){
	eventObject** events = (eventObject**)RAM::readPtr(&my_object->events);
	return (eventObject*)RAM::readPtr(&events[index]);
}	

uint32_t DisplayObject::getDelayTime(){
	return RAM::read32(&my_object->delayTime);
}

uint32_t DisplayObject::getDelayStartTime(){
	return RAM::read32(&my_object->delayStartTime);
}

uint8_t DisplayObject::isDelaying(){
	return RAM::read(&my_object->isDelaying);
}

uint8_t DisplayObject::getIndex(){
	return RAM::read(&my_object->index);
}

uint8_t DisplayObject::getChildIndexChanged(){
	return RAM::read(&my_object->childIndexChanged);
}

uint8_t DisplayObject::getChildRemoved(){
	return RAM::read(&my_object->childRemoved);
}

uint8_t DisplayObject::getMaskType(){
	return RAM::read(&my_object->maskType);
}

/*uint8_t DisplayObject::getEventInfo(){
	return RAM::read(&my_object->eventInfo);
}*/

uint8_t* DisplayObject::getExternalMask(){
	return RAM::readPtr(&my_object->externalMask);
}

//------------------------------------------ SETTER -------|
//------------------------------------------ ////// -------|

void DisplayObject::setExternalMask(uint8_t* mask){
	RAM::writePtr(&my_object->externalMask, mask);
}

/*void DisplayObject::setEventInfo(uint8_t value){
	RAM::write(&my_object->eventInfo, value);
}*/

void DisplayObject::setMaskType(uint8_t value){
	RAM::write(&my_object->maskType, value);
}

void DisplayObject::setChildRemoved(uint8_t value){
	RAM::write(&my_object->childRemoved, value);
}

void DisplayObject::setChildIndexChanged(uint8_t value){
	RAM::write(&my_object->childIndexChanged, value);
}

void DisplayObject::setIndex(uint8_t value){
	RAM::write(&my_object->index, value);
}

void DisplayObject::setDelaying(uint8_t value){
	RAM::write(&my_object->isDelaying, value);
	setDelayStartTime(millis());
}

void DisplayObject::setDelayStartTime(uint32_t time){
	RAM::write32(&my_object->delayStartTime, time);
}

void DisplayObject::setDelayTime(uint32_t time){
	RAM::write32(&my_object->delayTime, time);
}

void DisplayObject::setFlipped(uint8_t value){
	RAM::write(&my_object->flipped, value);
}

uint8_t DisplayObject::isFlipped(){
	return RAM::read(&my_object->flipped);
}

void DisplayObject::setCollider(uint8_t value){
	RAM::write(&my_object->collider, value);
}

void DisplayObject::setWidth(uint16_t value){
	RAM::write16(&my_object->width, value);
}

void DisplayObject::setHeight(uint16_t value){
	RAM::write16(&my_object->height, value);
}

void DisplayObject::setWidthHeight(uint16_t width, uint16_t height){
	setWidth(width);
	setHeight(height);
}

void DisplayObject::setId(uint16_t value){
	RAM::write(&my_object->id, value);
}

void DisplayObject::setName(char* name){
	uint8_t i = 0;
	char ch;
	char temp[NAME_MAX_LENGTH+1];
	
	do{
		ch = name[i];
		temp[i] = ch;
		i++;
		
		if(i>=NAME_MAX_LENGTH){
			temp[i] = '\0';
			i++;
			break;
		}
	}while(ch);
	
	char *name_ptr = (char*)RAM::malloc(i);
	RAM::write16(&my_object->name, (word)name_ptr);
	RAM::writeArray(name_ptr, temp, i);
}

void DisplayObject::setImage(uint8_t* image){
	RAM::writePtr(&my_object->image, image);
	if(getHeight() == 0)
		setHeight(getCanvasHeight());
	
	if(getWidth() == 0)
		setWidth(getCanvasWidth());
}

void DisplayObject::setX(int16_t x){
	RAM::write16(&my_object->x, x);
}

void DisplayObject::setY(int16_t y){
	RAM::write16(&my_object->y, y);
}

void DisplayObject::setXY(int16_t x, int16_t y){
	setX(x);
	setY(y);
}

void DisplayObject::setGlobalX(int16_t globalX){
	RAM::write16(&my_object->globalX, globalX);
}

void DisplayObject::setGlobalY(int16_t globalY){
	RAM::write16(&my_object->globalY, globalY);
}

void DisplayObject::setParent(DisplayObject* parent){
	RAM::writePtr(&my_object->parent, parent);
}

void DisplayObject::setVisibility(uint8_t visibility){
	RAM::write(&my_object->visibility, visibility);
}

void DisplayObject::setMemory(uint8_t memory){
	RAM::write(&my_object->memory, memory);
}

void DisplayObject::setChildCount(uint8_t childCount){
	RAM::write(&my_object->childCount, childCount);
}

void DisplayObject::setChildAt(uint8_t index, DisplayObject* child){
	DisplayObject** children = (DisplayObject**)RAM::read16(&my_object->children);
	RAM::writePtr(&children[index], child);
}

void DisplayObject::setDrawing(uint8_t value){
	RAM::write(&my_object->drawing, value);
}


//------------------------------------------ GETTER -------|
//------------------------------------------ ////// -------|

void DisplayObject::getName(char* buffer, uint8_t length){ //array length (The number of empty places that i have)
	if(length < 1)
		return;
	
	length--; //Last place is for terminating
	char* name = (char*)RAM::read16(&my_object->name);
	
	if(name != NULL)
	{	
		uint8_t i = 0;
		char ch;
		
		do{
			if(i>=length){
				buffer[i] = '\0';
				break;
			}
			
			ch = (char)RAM::read(&name[i]);
			buffer[i] = ch;
			i++;	
		}while(ch);
	}
	else{
		char no[] = "Noname";
		uint8_t i = 0;
		char ch;
		do{
			ch = no[i];
			buffer[i] = ch;
			i++;
			
			if(i>=length || i==7){
				buffer[i] = '\0';
				break;
			}
		}
		while(ch);	
	}
}

DisplayObject* DisplayObject::getChildAt(uint8_t index){
	if(index >= getChildCount())
		return NULL;
	
	DisplayObject** children = (DisplayObject**)RAM::readPtr(&my_object->children);
	return (DisplayObject*)RAM::readPtr(&children[index]);
}

uint16_t DisplayObject::getId(){
	return RAM::read(&my_object->id);
}

uint8_t* DisplayObject::getImage(){
	return (uint8_t*)RAM::readPtr(&my_object->image);
}

int16_t DisplayObject::getX(){
	return RAM::read16(&my_object->x);
}

int16_t* DisplayObject::getXRef(){
	return &my_object->x;
}

int16_t DisplayObject::getY(){
	return RAM::read16(&my_object->y);
}

int16_t* DisplayObject::getYRef(){
	return &my_object->y;
}

int16_t DisplayObject::getGlobalX(){
	return RAM::read16(&my_object->globalX);
}

int16_t DisplayObject::getGlobalY(){
	return RAM::read16(&my_object->globalY);
}

uint16_t DisplayObject::getCanvasWidth(){
	if(getMemory() == SPIMEM)
		return RAM::read16(&getImage()[0]);
	else
		return pgm_read_word(&getImage()[0]);
}

uint16_t DisplayObject::getCanvasHeight(){
	if(getMemory() == SPIMEM)
		return RAM::read16(&getImage()[2]);
	else
		return pgm_read_word(&getImage()[2]);
}

uint16_t DisplayObject::getWidth(){
	return RAM::read16(&my_object->width);
}

uint16_t DisplayObject::getHeight(){
	return RAM::read16(&my_object->height);
}

DisplayObject* DisplayObject::getParent(){
	return (DisplayObject*)RAM::readPtr(&my_object->parent);
}

uint8_t DisplayObject::isVisible(){
	return RAM::read(&my_object->visibility);
}

uint8_t DisplayObject::getMemory(){
	return RAM::read(&my_object->memory);
}

uint8_t DisplayObject::getChildCount(){
	return RAM::read(&my_object->childCount);
}

uint8_t DisplayObject::getChildSize(){
	return RAM::read(&my_object->childSize);
}

uint8_t DisplayObject::isDrawing(){
	return RAM::read(&my_object->drawing);
}

uint8_t DisplayObject::getCollider(){
	return RAM::read(&my_object->collider);
}

void DisplayObject::writeSerialInfo(){
	/*object temp = {};
	loadObject(&temp, my_object);
	
	Serial.print(F("Name: "));
	char name[NAME_MAX_LENGTH+1];
	getName(name,NAME_MAX_LENGTH+1);
	Serial.println(name);
	
	Serial.print(F("Parent name: "));
	if(temp.parent){
		temp.parent->getName(name, NAME_MAX_LENGTH+1);
		Serial.println(name);
	}else{
		Serial.println(F("No parent attached"));
	}
	
	Serial.print(F("Child size: "));
	Serial.println(temp.childSize);
	
	Serial.print(F("Child count: "));
	Serial.println(temp.childCount);
	
	if(temp.childCount>0){
		Serial.print(F("My children names :"));
		for(uint8_t i=0; i<temp.childCount; i++){
			getChildAt(i)->getName(name, NAME_MAX_LENGTH+1);
			Serial.print(name);
			if(i!=temp.childCount-1)
				Serial.print(F(", "));
			else
				Serial.println(F(""));
		}
	}
	
	Serial.print(F("Position global x: "));
	Serial.println(temp.globalX);
	
	Serial.print(F("Position global y: "));
	Serial.println(temp.globalY);
	
	Serial.print(F("Position local x: "));
	Serial.println(temp.x);
	
	Serial.print(F("Position local y: "));
	Serial.println(temp.y);
	
	Serial.println(F("----"));*/
}
