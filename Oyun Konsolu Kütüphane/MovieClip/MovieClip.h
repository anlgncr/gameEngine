#ifndef MovieClip_h
#define MovieClip_h
#include "arduino.h"
#include "DisplayObject.h"
#include "RAM.h"
#include "Convert.h"

class MovieClip : public DisplayObject{
	public:
		struct object{
			uint8_t FPS;
			uint8_t frameCount;
			uint8_t repeatCount;
			uint8_t currentFrame;
			uint8_t isPlaying;
			uint8_t **images;
			uint32_t *frameChangeTimes;
			uint32_t passedTime;	
			uint8_t firstFrame;
			uint8_t lastFrame;
		};
		
		MovieClip(uint8_t);
		MovieClip();
		
		bool createFromFile(char* fileName);
		bool createFromProgmem(uint8_t **images, uint8_t frameCount, uint8_t FPS);
		void createInstanceOf(MovieClip*);
		void copy(MovieClip*);
		
		void onUpdate();
		void writeInfo();
		void play();
		void playFor(uint8_t);
		void playOnce();
		void gotoAndPlay(uint8_t);
		void stop();
		void gotoAndStop(uint8_t);
		void nextFrame();
		void prevFrame();
		void setFirstAndLastFrame(uint8_t, uint8_t);
		void setFPS(uint8_t);
		
		uint8_t isPlaying();
		uint8_t getFPS();
		uint8_t getFrameCount();
		uint8_t getCurrentFrame();
		uint8_t getRepeatCount();
		uint8_t getFirstFrame();
		uint8_t getLastFrame();
		uint8_t** getImages();
		object* getMovieClipObject();
	
	private:
		object *my_object;
			
		bool createMovieClip(uint8_t**, uint8_t, uint8_t);
		
		void copyObject(object*, object*);
		void loadObject(object*, object*);
		void playMovie();
		uint32_t getPassedTime();
		uint8_t* getImageAt(uint8_t); //SPI RAM için
		
		uint32_t getFrameChangeTime(uint8_t);
		void setRepeatCount(uint8_t);
		void setPassedTime(uint32_t);
		void setPlaying(uint8_t);
		void setCurrentFrame(uint8_t);
};

#endif