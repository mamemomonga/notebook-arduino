#include <ESP8266WiFi.h>
#include <Wire.h>

#include "Config.h"
#include "MainApp.h"

MainAppClass mainapp;

void setup() {
	Serial.println(""); 
	Serial.println(""); 
	Serial.println("----- STARTUP -----"); 
	WiFi.mode(WIFI_OFF);

	Serial.begin(115200);
	pinMode(LED_STATUS,OUTPUT);
	digitalWrite(LED_STATUS,HIGH);

	delay(1000);
	mainapp.init();
}

void loop() {
	mainapp.handle();
}

