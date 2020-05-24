#ifndef GameObject_h
#define GameObject_h
#include "Sprite.h"

class GameObject : public Sprite
{
	public:
		GameObject(): Sprite()			
		{
			drawFromFile("o.b");	
		}	
		
		void onUpdate(){
			move();
		}
			
	private:
			uint8_t left, right, down, up;
			
			void move()
			{
				if(left){
					setX(getX() - 1);
				}
				else if(right){
					setX(getX() + 1);
				}
				else if(up){
					setY(getY() - 1);
				}
				else if(down){
					setY(getY() + 1);
				}
			}
			
			void setButtonStatus(uint8_t button, uint8_t value){
				if(button == BUTTON_H){
					left = value;
				}
				else if(button == BUTTON_F){
					right = value;
				}
				else if(button == BUTTON_G){
					up = value;
				}
				else if(button == BUTTON_E){
					down = value;
				}
			}
			
			void onButtonUp(uint8_t button){
				setButtonStatus(button, false);
			}
			
			void onButtonDown(uint8_t button){
				setButtonStatus(button, true);				
			}
			
			void onButtonLongDown(uint8_t button){}
};

#endif