#ifndef DisplayObject_h
#define DisplayObject_h
#include "arduino.h"
#include "RAM.h"
#include "Button.h"
#include "EventNames.h"
#include "EventArgs.h"

#define NAME_MAX_LENGTH 15
#define COLLIDER_RECTANGLE 0
#define COLLIDER_CIRCLE 1

#define IMAGE_NO_MASK 0
#define IMAGE_SELF_MASK 1
#define IMAGE_AUTO_MASK 2
#define IMAGE_EXTERNAL_MASK 3

const uint8_t noImage[] PROGMEM = { 16, 0, 8, 0,
  0xFF, 0x81, 0xC3, 0xC3, 0xA5, 0xA5, 0x99, 0x99, 0x99, 0x99, 0xA5, 0xA5, 0xC3, 0xC3, 0x81, 0xFF
};

class Manager;
class DisplayObject{
	public:		
		DisplayObject(uint8_t);
		typedef void(*eventFunction)(EventArgs*);

		// Displayobject'in pozisyonunu zamana göre değiştirir.
		// Saniyede x piksel yer değiştir.
		void moveX(int16_t);
		void moveY(int16_t);
		
		void moveTo(int16_t x, int16_t y, uint32_t time);
		void moveTo(int16_t x, int16_t y, uint32_t time, uint8_t animation);
		
		void dispatchEvent(uint8_t);
		void dispatchEvent(EventArgs*);
		void dispatchEventAll(uint8_t);
		void dispatchEventAll(EventArgs*);
		void addEventListener(uint8_t, eventFunction);
		void removeEventListener(uint8_t);
		void writeSerialInfo();
		
		virtual void onSpecificEvent(uint8_t){};
		virtual void onDelay(){};
		virtual void onUpdate(){};
		virtual void onAdded(){};
		virtual void onRemoved(){};
		virtual void onButtonDown(uint8_t){};
		virtual void onButtonUp(uint8_t){};
		virtual void onButtonLongDown(uint8_t){};
		
		
		uint8_t hitTest(DisplayObject*);
		uint16_t distanceTo(DisplayObject*);
		
		void applyChildChanges();
		
		void setId(uint16_t);
		void setName(char*);
		void addChild(DisplayObject*);
		void removeChild(DisplayObject*);
		void remove();
		void leaveParent();
		void setImage(uint8_t*);
		void setX(int16_t); 
		void setY(int16_t);
		void setXY(int16_t, int16_t);
		void setWidth(uint16_t);
		void setHeight(uint16_t);
		void setWidthHeight(uint16_t, uint16_t);
		void setGlobalX(int16_t); 
		void setGlobalY(int16_t);
		void setVisibility(uint8_t);
		void setMemory(uint8_t);
		void setDrawing(uint8_t);
		void setCollider(uint8_t);
		void setFlipped(uint8_t);
		void setDelayTime(uint32_t);
		void setDelayStartTime(uint32_t);
		void setDelaying(uint8_t);
		void setChildIndex(DisplayObject*, uint8_t);
		void setMaskType(uint8_t);
		void setEventInfo(uint8_t);
		void setExternalMask(uint8_t*);
		
		uint16_t getId();
		void getName(char*, uint8_t);
		DisplayObject* getParent();
		bool contains(DisplayObject*);
		DisplayObject* getChildAt(uint8_t);
		uint8_t* getImage();
		int16_t getX();
		int16_t getY();
		uint16_t getWidth();
		uint16_t getHeight();
		uint16_t getCanvasWidth();
		uint16_t getCanvasHeight();
		int16_t* getXRef();
		int16_t* getYRef();
		int16_t getGlobalX();
		int16_t getGlobalY();
		uint8_t isVisible();
		uint8_t getChildCount();
		uint8_t getChildSize();
		uint8_t getMemory();
		uint8_t isDrawing();
		uint8_t getCollider();
		uint8_t isFlipped();
		uint32_t getDelayTime();
		uint32_t getDelayStartTime();
		uint8_t isDelaying();
		uint8_t getIndex();
		uint8_t getMaskType();
		uint8_t getEventInfo();
		uint8_t* getExternalMask();

	private:
		/*
			eventFunction function -> Nesnenin çağıracağı fonksiyon (Fonksiyon static olmalı)
			uint8_t event -> Nesnenin dinlediği olayın adı
		*/
		struct eventObject{
			eventFunction function;
			uint8_t event;
		};
		
		/*
			uint16_t id -> Kullanıcı tarafından atanan isteğe bağlı kimlik
			char* name -> Nesnenin ismi. Eğer isim atanmamışsa varsayılan isim "noname"
			DisplayObject *parent -> Bu nesnenin bağlı olduğu bir üst nesne
			DisplayObject **children -> Bu nesnenin sahip olduğu alt nesneler
			uint8_t *image -> Nesnenin bitmap görüntüsü
			uint8_t *externalMask -> Nesnenin görüntüsünün isteğe bağlı maskesi
			uint8_t maskType -> Nesneye bağlı görüntünün maske türü
			int16_t x -> Nesnenin bağlı olduğu nesneye göre x konumu
			int16_t y -> Nesnenin bağlı olduğu nesneye göre y konumu
			int16_t globalX -> Nesnenin ekranda çizildiği x konumu
			int16_t globalY -> Nesnenin ekranda çizildiği y konumu
			uint16_t height -> Bitmap görüntüsünün yüksekliği
			uint16_t width -> Bitmap görüntüsünün genişliği
			uint8_t flipped -> Bitmap'i yatay çevirir
			uint8_t index -> Nesnenin bağlı bulunduğu nesnedeki dizin numarası
			uint8_t childSize -> Bu nesnenin sahip olabileceği azami nesne sayısı 
			uint8_t childCount -> Bu nesneye bağlı bulunan nesne sayısı
			uint8_t visibility -> Nesne sahnede görünür mü?
			uint8_t memory -> Bu nesnenin bitmap görüntüsünün bulunduğu hafıza
			uint8_t drawing -> Nesne sahnede çiziliyor mu?
			uint8_t eventInfo -> DispatchEvent ile veri göndermek için
			eventObject **events -> Nesnenin dinlediği olaylar 
			uint8_t eventSize -> En fazla kaç olay dinleyebilir
			uint8_t eventCount -> Şuanda kaç olay dinliyor
			uint8_t collider -> Nesnenin çarpışma algılaması için şekli. Dörtgen veya daire
			uint32_t delayStartTime -> Nesnenin belirlenen araklıklarla çağırdığı fonksiyonun son çağrılma zamanı
			uint32_t delayTime -> Nesnenin çağırdığı fonksiyonun gecikme zamanı
			uint8_t isDelaying -> Nesnedeki gecikme fonksiyonu var mı?
			uint8_t childIndexChanged -> Sahip olduğu nesnelerin sırası değişti mi?
			uint8_t childRemoved -> Sahip olduğu nesnelerden biri silindi mi?
		*/
		
		struct object{
			uint16_t id;
			char* name;
			DisplayObject *parent;
			DisplayObject **children;
			uint8_t *image;
			uint8_t *externalMask;
			uint8_t maskType;
			int16_t x;
			int16_t y;
			int16_t globalX;
			int16_t globalY;
			uint16_t height;
			uint16_t width;
			uint8_t flipped;
			uint8_t index;
			uint8_t childSize;
			uint8_t childCount;
			uint8_t visibility;
			uint8_t memory;
			uint8_t drawing;
			eventObject **events;
			uint8_t eventSize;
			uint8_t eventCount;
			uint8_t collider;
			uint32_t delayStartTime;
			uint32_t delayTime;
			uint8_t isDelaying;
			uint8_t childIndexChanged;
			uint8_t childRemoved;
		};
		object *my_object;
		
		void copyObject(object*, object*);
		void loadObject(object*, object*);
		void disposeChild();
		uint16_t getRadius();
		
		void updateEvent(EventArgs*);
		
		void setChildIndexChanged(uint8_t);
		uint8_t getChildIndexChanged();
		
		void setChildRemoved(uint8_t);
		uint8_t getChildRemoved();
		
		void setParent(DisplayObject*);
		void setChildAt(uint8_t, DisplayObject*);
		void setChildCount(uint8_t);
		void setIndex(uint8_t);
		
		void setEvent(eventObject*, uint8_t);
		void setEventFunction(eventObject*, eventFunction);	
		void setEventAt(uint8_t, eventObject*);
		void setEventSize(uint8_t);
		void setEventCount(uint8_t);
		void disposeEvent();
		
		uint8_t getEvent(eventObject*);
		eventFunction getEventFunction(eventObject*);
		eventObject* getEventAt(uint8_t);
		uint8_t getEventSize();
		uint8_t getEventCount(); 
};

#endif