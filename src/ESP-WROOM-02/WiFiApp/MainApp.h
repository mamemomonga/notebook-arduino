#ifndef _MAINAPP_H_
#define _MAINAPP_H_

#include <ESP8266WiFi.h>

#include "Arduino.h"
#include "Config.h"
#include "Storage.h"
#include "WebData.h"

#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <DNSServer.h>

class MainAppClass {
	public:
		MainAppClass();
		StorageClass storage;
		void init();
		void handle();

	private:
		void setup_webserver();
		void setup_ap();
		void sta_connect();

		uint8_t sta;
		uint8_t sta_ct;
		unsigned long last_millis;

		String tmpString;
		void hard_reset();

	protected:
};

#endif
