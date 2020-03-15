#include "Storage.h"

StorageClass::StorageClass() {
	_init();
}

void StorageClass::_init() {
	Serial.println(F("[Storage] Init"));
	for(uint8_t i=0;i<4;i++) {
		WifiConnectionInfo[i].ssid[0]      = '\0';
		WifiConnectionInfo[i].passphase[0] = '\0';
	}
}

uint8_t StorageClass::_write(char header[5]) {
	EEPROM.begin(STORAGE_EEPROM_START_ADDR);
	int addr=0; 

	for(uint8_t i=0; i<5; i++) {
		EEPROM.write(addr,header[i]);
		addr++;
	}
	for(uint8_t j=0; j<4; j++) {
		for(uint8_t i=0; i<32; i++) {
			EEPROM.write(addr,WifiConnectionInfo[j].ssid[i]);
			addr++;
		}
		for(uint8_t i=0; i<64; i++) {
			EEPROM.write(addr,WifiConnectionInfo[j].passphase[i]);
			addr++;
		}
	}
	EEPROM.commit();
	return 1;
}


uint8_t StorageClass::_read(char header[5]) {
	EEPROM.begin(STORAGE_EEPROM_START_ADDR);
	int addr=0; 

	char hdr[5];
	for(uint8_t i=0; i<5; i++) {
		hdr[i]=EEPROM.read(addr);
		addr++;
	}

	if(strcmp(header,hdr)) {
		return 0;
	}

	for(uint8_t j=0; j<4; j++) {
		for(uint8_t i=0; i<32; i++) {
			WifiConnectionInfo[j].ssid[i]=EEPROM.read(addr);
			addr++;
		}
		for(uint8_t i=0; i<64; i++) {
			WifiConnectionInfo[j].passphase[i]=EEPROM.read(addr);
			addr++;
		}
	}
	return 1;
}

uint8_t StorageClass::load() {
	Serial.println(F("[Storage] Load EEPROM"));

	if(_read("REDY")) {
		Serial.println(F("[Storage] Data OK"));
		return 1;
	} else {
		Serial.println(F("[Storage] Data NG"));
		return 0;
	}
}

void StorageClass::save() {
	Serial.println(F("[Storage] Save EEPROM"));
	_write("REDY");
}

void StorageClass::clear() {
	_init();
	Serial.println(F("[Storage] Clear EEPROM"));
	_write("FAIL");
}
