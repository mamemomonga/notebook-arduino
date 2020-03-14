#include <ESP8266WiFi.h>
#include <Wire.h>

#include "Config.h"
#include "MainApp.h"

MainAppClass mainapp;

void setup() {
	Serial.begin(115200);
	pinMode(LED_STATUS,OUTPUT);
	digitalWrite(LED_STATUS,HIGH);
	Serial.println("Setup");
	mainapp.setup();
}

void loop() {
	Serial.println("Running Main");
	digitalWrite(LED_STATUS,HIGH);
	delay(500);
	digitalWrite(LED_STATUS,LOW);
	delay(500);
}

