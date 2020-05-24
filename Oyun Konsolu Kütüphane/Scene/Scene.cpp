#include "Scene.h"
#include "Manager.h"

Scene::Scene(uint8_t childSize) : DisplayObject(childSize){}
				
void Scene::onUpdate()
{
	myScreen.clearBuffer();
	updateChild(this);
	myScreen.writeBuffer();
	adjustBrightness();
}
		
void Scene::updateChild(DisplayObject* child)
{
	if(child == NULL)
		return;

	if(child->isVisible()){
		if(myScreen.testDraw(child)){
			myScreen.draw();
		}
		
		if(child->getChildCount() > 0){
			for(uint8_t i=0; i<child->getChildCount(); i++){					
				updateChild(child->getChildAt(i));
			}
		}
	}
}

void Scene::setInverse(uint8_t value){
	myScreen.setInverse(value);
}

void Scene::adjustBrightness(){
	if(autoBrightness){
		myScreen.setBrightness(Manager::getBrightness());
	}
}
		
void Scene::onButtonDown(uint8_t button){
	if(button == CODE_X){
		setInverse(inverse);
		inverse = !inverse;
	}
	
	if(button == CODE_Y){
		if(sleeping)
			wakeUp();
		else
			sleep();
		
		sleeping = !sleeping;
	}
}

bool Scene::isSleeping(){
	return sleeping;
}
		
void Scene::sleep(){
	myScreen.sleep();
}

void Scene::wakeUp(){
	myScreen.wakeUp();
}
		