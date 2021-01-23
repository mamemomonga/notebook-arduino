#include <ESP8266WiFi.h>
#include "Config.h"

#include <ESP8266WiFiMulti.h>

#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <DNSServer.h>

#include "WiFiSetup.h"
#include <Wire.h>

void setup() {
	pinMode(LED_STATUS,OUTPUT);
	digitalWrite(LED_STATUS,HIGH);

	Serial.begin(115200);
	delay(500);

	Serial.println();
	Serial.println();
	Serial.println();
	Serial.println("[SERIAL READY]");

	WiFiSetupClass wifisetup;

	// IO12がLOWなら、EEPROMをクリアする
	pinMode(12,INPUT_PULLUP);
	if( digitalRead(12) == LOW ) {
		wifisetup.clear_eeprom();
	}

	// IO0(Programボタン)＝WiFi接続中断
	pinMode(0,INPUT);
	wifisetup.begin();
}

void loop() {
	Serial.println("Running Main");
	digitalWrite(LED_STATUS,HIGH);
	delay(500);
	digitalWrite(LED_STATUS,LOW);
	delay(500);
}

