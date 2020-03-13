#ifndef _WIFI_CONFIG_H_
#define _WIFI_CONFIG_H_

#include "Arduino.h"
#include "Config.h"
#include "Storage.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>

class WiFiConfigClass {
	public:
		WiFiConfigClass();
		void begin();

	private:
		void _setup_wifi_info();
		uint8_t _wifi_scan();
		uint8_t _connect_to_wifi();
		uint8_t _server_enable=1;

		ESP8266WebServer *server = nullptr;
		void send_error();
		void send_success();

		StorageClass storage;

	protected:
};

#endif
