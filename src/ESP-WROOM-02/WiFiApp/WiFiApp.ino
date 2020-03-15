#include <ESP8266WiFi.h>
#include <Wire.h>

#include "Config.h"
#include "MainApp.h"

MainAppClass mainapp;
unsigned long ct_mlast;
uint8_t ct_1000;
uint8_t ct_500;

void status_blink() {
	if( digitalRead(LED_STATUS) ) {
		digitalWrite(LED_STATUS,LOW);
	} else {
		digitalWrite(LED_STATUS,HIGH);
	}
}

void setup() {
	Serial.println(""); 
	Serial.println(""); 
	Serial.println("----- STARTUP -----"); 
	WiFi.mode(WIFI_OFF);

	Serial.begin(115200);
	pinMode(LED_STATUS,OUTPUT);
	digitalWrite(LED_STATUS,HIGH);

	pinMode(12,INPUT_PULLUP);

	if(digitalRead(12) == LOW) {
		Serial.println("DISABLE WIFI");
		mainapp.disable_wifi=true;
		WiFi.disconnect();
		WiFi.mode(WIFI_OFF);
		WiFi.forceSleepBegin();

	} else {
		Serial.println("ENABLE WIFI");
		mainapp.disable_wifi=false;
	}

	delay(1000);
	mainapp.setup();

	ct_mlast=0;
	ct_1000=0;
}

void loop() {
	// every tick

	mainapp.handles1();

 	unsigned long mi=millis();
	if((mi-ct_mlast) < 50) { return; }
	ct_mlast=mi;

	// 50ms

	// 500ms
 	if(ct_500 > 10) {
		status_blink();
		ct_500=0;
	}

	// 1000ms
	if(ct_1000 > 20) {
		mainapp.handles2();
		ct_1000=0;
	}

	ct_1000++;
	ct_500++;
}
