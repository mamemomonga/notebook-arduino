#include <ESP8266WiFi.h>
#include "Config.h"

#include "WiFiConfig.h"
#include <Wire.h>

void setup() {
	pinMode(LED_STATUS,OUTPUT);
	digitalWrite(LED_STATUS,HIGH);

	Serial.begin(115200);
	delay(500);

	Serial.println();
	Serial.println();
	Serial.println("[SERIAL READY]");

	WiFiConfigClass wifi_config;

	// IO0(Programボタン)＝WiFi接続中断
	pinMode(0,INPUT);
	wifi_config.begin();
}

void loop() {
	Serial.println("Running Main");
	digitalWrite(LED_STATUS,HIGH);
	delay(500);
	digitalWrite(LED_STATUS,LOW);
	delay(500);
}

