#ifndef _MAINAPP_H_
#define _MAINAPP_H_

#include <ESP8266WiFi.h>

#include "Arduino.h"
#include "Config.h"
#include "Storage.h"
#include "WebData.h"

#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <DNSServer.h>
// #include <ESP8266mDNS.h>

class MainAppClass {
	public:
		MainAppClass();
		StorageClass storage;
		void setup();
	private:
		void wifi_setup();
		uint8_t wifi_connect();
	protected:
};

#endif
