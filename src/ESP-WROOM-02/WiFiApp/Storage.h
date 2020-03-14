#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "Arduino.h"
#include <EEPROM.h>

#define STORAGE_EEPROM_START_ADDR 512

typedef struct {
	char ssid[32];
	char passphase[64];
} WiFiConnectionInfo_t;

class StorageClass {
	public:
		StorageClass();
		WiFiConnectionInfo_t WifiConnectionInfo[6];
		uint8_t load();
		void save();
		void clear();

	private:
		void     _init();
		uint8_t  _read(char[5]);
		uint8_t _write(char[5]);

	protected:
};

#endif
