#ifndef _WIFI_SETUP_H_
#define _WIFI_SETUP_H_

#include "Config.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <DNSServer.h>

#include "Arduino.h"

#define EEPROM_START_ADDR 512

typedef struct {
	char ssid[32];
	char passphase[64];
} WiFiConnectionInfo_t;

class WiFiSetupClass {
	public:
		WiFiSetupClass();
		void begin();
		void clear_eeprom();

	private:
		void _setup_wifi_info();
		uint8_t _connect_to_wifi();
		uint8_t _read_config_eeprom();
		void _write_config_eeprom();

		uint8_t _server_enable=1;
		uint8_t _eeprom_data_ok=0;

		WiFiConnectionInfo_t wci[6];

	protected:
};

#endif
