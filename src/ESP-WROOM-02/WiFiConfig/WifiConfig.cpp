#include "WiFiConfig.h"
#include "WebData.h"

WiFiConfigClass::WiFiConfigClass() {
	for(uint8_t i=0;i<4;i++) {
		strcpy(wci[i].ssid,"");
		strcpy(wci[i].passphase,"");
	}
}

void WiFiConfigClass::begin() {

	delay(100);
	pinMode(12,INPUT_PULLUP);

	while(1) {

		if( digitalRead(FORCE_CONFIG_PIN) == LOW ) {
			_setup_wifi_info();
		}

		if(_read_config_eeprom()) { // 0:成功
			_setup_wifi_info();
		}

		// 接続開始
		if(_connect_to_wifi()) { // 0:成功
			_setup_wifi_info();

		} else {
			// 接続成功
			break;
		}
	}
	delay(1000);
}

// WiFiネットワークのスキャン
uint8_t WiFiConfigClass::_wifi_scan() {
	uint8_t networksFound = WiFi.scanNetworks(false,false);
	Serial.printf("%d network(s) found\r\n", networksFound);
	for (int i = 0; i < networksFound; i++) {
		Serial.printf("%d: %s, Ch:%d (%ddBm) %s\r\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
	}
	return networksFound;
}

void WiFiConfigClass::_setup_wifi_info() {
	// CaptivePortal
	_server_enable=1;

	ESP8266WiFiClass softap;
	ESP8266WebServer server(80);

	DNSServer dnsServer;

	WiFi.mode(WIFI_STA);
	Serial.printf(" -- WIFI SETUP WEB --\r\n");
	Serial.printf("SSID: %s\r\n", WIFISETUP_SSID );


	IPAddress apIP(WIFISETUP_IPADDRESS);
	softap.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	softap.softAP( WIFISETUP_SSID );

	delay(100);

	IPAddress myIP = softap.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);

	dnsServer.start(53, "*", apIP);
	Serial.println("DNS Server started");

	WebDataClass web_data;

	server.on("/",[&](){
		Serial.println("[SERVE] /");
		server.send(200, "text/html",web_data.WiFiSetupIndex());
		return;
	});

	server.on("/api/ap/config",[&](){
		Serial.println("[SERVE] /api/ap/config");
		if( server.method() != HTTP_POST ) {
			server.send(200, "text/json","{ \"error\": 1 }");
			return;
		}
		for(uint8_t i=0; i<=4; i++) {
			char name_ssid[6];  snprintf(name_ssid,6, "ssid%d",i);
			char name_pass[11]; snprintf(name_pass,11,"passphase%d",i);
			char name_upd[8];   snprintf(name_upd, 8, "update%d",i);
			char ssid[31];
			char passphase[63];

			uint8_t update=server.arg(name_upd).toInt();
			server.arg(name_ssid).toCharArray(ssid,31);
			server.arg(name_pass).toCharArray(passphase,63);

			if(update == 1) {
				strcpy(wci[i].ssid,      ssid);
				strcpy(wci[i].passphase, passphase);
				Serial.printf("Update Entry %d: SSID: %s\r\n",i,wci[i].ssid,ssid);
			}
		}
		_write_config_eeprom();
		server.send(200, "text/json","{ \"error\": 0 }");
		_server_enable=0;
		return;
	});

	server.on("/api/ap/reset",[&](){
		Serial.println("[SERVE] /api/ap/reset");
		if( server.method() != HTTP_POST ) {
			server.send(200, "text/json","{ \"error\": 1 }");
			return;
		}
		if( server.arg("execute_reset").toInt() == 42 ) {
			_clear_eeprom();
			server.send(200, "text/json","{ \"error\": 0 }");
			_server_enable=0;
		} else {
			server.send(200, "text/json","{ \"error\": 1 }");
		}
		return;
	});

	server.on("/api/ap/scan",[&](){
		Serial.println("[SERVE] /api/ap/scan");

		// WiFiスキャン
		WiFi.disconnect();
		delay(100);
		uint8_t networksFound = WiFi.scanNetworks(false,false);
		Serial.printf("%d network(s) found\r\n", networksFound);

		for (int i = 0; i < networksFound; i++) {
			Serial.printf("%d: %s, Ch:%d (%ddBm) %s\r\n",
				i + 1,
				WiFi.SSID(i).c_str(),
				WiFi.channel(i),
				WiFi.RSSI(i),
				WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : ""
			);
		}
		delay(100);

		String buf="{ \"result\": 0, \"data\": [";
		for (int i = 0; i < networksFound; i++) {
			// Openネットワークは表示しない
			if( WiFi.encryptionType(i) == ENC_TYPE_NONE ) { continue; }
			char b[255];
			snprintf_P(b,sizeof(b),PSTR("{\"ssid\":\"%s\",\"channel\":%d,\"rssi\":%d}"),
				WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i));
			buf=buf+b;
			if(i == (networksFound-1)) { buf=buf+"]}"; } else { buf=buf+","; }
		}
		server.send(200, "text/json",buf);
	});

	server.on("/api/ap/list",[&](){
		Serial.println("[SERVE] /api/ap/current");
		_read_config_eeprom();

		String buf="{ \"result\": 0, \"data\": [";
		for (int i = 0; i < 4; i++) {
			char b[255];
			snprintf_P(b,sizeof(b),PSTR("{\"ssid\":\"%s\"}"),wci[i].ssid);
			buf=buf+b;
			if(i == 3) { buf=buf+"]}";} else { buf=buf+","; }
		}
		server.send(200, "text/json",buf);
	});

	server.onNotFound([&]() {
		Serial.println("[SERVE] 404");
		char redirect[32];
		snprintf_P(redirect,sizeof(redirect),PSTR("http://%d.%d.%d.%d/"),myIP[0],myIP[1],myIP[2],myIP[3]);
		server.sendHeader("Location", redirect, true);
		server.send(301,"text/plain","");
	});

	server.begin();
	Serial.println("HTTP server started");

	unsigned long last_millis=0;

	// サーバプロセスとLED点滅
	while(_server_enable) {

		dnsServer.processNextRequest();
		server.handleClient();

		unsigned long current_millis=millis();
		if( ( current_millis - last_millis ) > 50 ) {
			if( digitalRead(LED_STATUS) ) { digitalWrite(LED_STATUS,LOW); } else { digitalWrite(LED_STATUS,HIGH); }
			last_millis=current_millis;
		}
	}
}

uint8_t WiFiConfigClass::_connect_to_wifi() {
	Serial.printf(" -- CONNECT TO WIFI --\r\n");
	ESP8266WiFiMulti wifiMulti;

	for(uint8_t i=0;i<4;i++) {
		if( wci[i].ssid[0] != '\0' ) {
			Serial.print("ADD SSID:"); Serial.println( wci[i].ssid );
			wifiMulti.addAP(wci[i].ssid, wci[i].passphase);
		}
	}

	Serial.printf("Connecting...\r\n");

	while(wifiMulti.run() != WL_CONNECTED) {
		if( digitalRead(FORCE_CONFIG_PIN) == LOW ) { return 1; }
		digitalWrite(LED_STATUS,LOW);
		delay(200);
		digitalWrite(LED_STATUS,HIGH);
		delay(200);
	}

	IPAddress ip = WiFi.localIP();
	uint8_t * mac = WiFi.BSSID();
	Serial.printf("Connected!\r\n");
	Serial.printf("  SSID: %s\r\n",WiFi.SSID().c_str());
	Serial.printf("  Channel: %d\r\n", WiFi.channel());
	Serial.printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	Serial.printf("  IP: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
	return 0;

}

// length = (32+64)*4 = 384byte
uint8_t WiFiConfigClass::_read_config_eeprom() {
	Serial.printf("-- Read EEPROM --\r\n");
	EEPROM.begin(EEPROM_START_ADDR);
	int addr=0; 

	char eeprom_data_ok[4]="";

	for(uint8_t i=0;i<4;i++) {
		eeprom_data_ok[i]=EEPROM.read(addr);
		addr++;
	}

	if(strncmp(eeprom_data_ok,"REDY",4)!=0) {
		_eeprom_data_ok=0;
		Serial.println("EEPROM_DATA_NG");
		for(uint8_t j=0;j<4;j++) {
			for(uint8_t i=0;i<32;i++) { wci[j].ssid[i]='\0'; }
			for(uint8_t i=0;i<64;i++) { wci[j].passphase[i]='\0'; }
		}
		return 1;
	}

	_eeprom_data_ok=1;
	Serial.println("EEPROM_DATA_OK");

	for(uint8_t j=0;j<4;j++) {
		for(uint8_t i=0;i<32;i++) {
			wci[j].ssid[i]=EEPROM.read(addr);
			addr++;
		}
		for(uint8_t i=0;i<64;i++) {
			wci[j].passphase[i]=EEPROM.read(addr);
			addr++;
		}
	}




	return 0;
}

void WiFiConfigClass::_write_config_eeprom() {
	Serial.printf("-- Write EEPROM --\r\n");
	EEPROM.begin(EEPROM_START_ADDR);
	int addr=0; 

	char eeprom_data_ok[]="REDY";
	for(uint8_t i=0;i<4;i++) {
		EEPROM.write(addr,eeprom_data_ok[i]);
		addr++;
	}
	for(uint8_t j=0;j<4;j++) {
		for(uint8_t i=0;i<32;i++) {
			EEPROM.write(addr,wci[j].ssid[i]);
			addr++;
		}
		for(uint8_t i=0;i<64;i++) {
			EEPROM.write(addr,wci[j].passphase[i]);
			addr++;
		}
	}
	EEPROM.commit();
}

void WiFiConfigClass::_clear_eeprom() {
	Serial.printf("-- Clear EEPROM --\r\n");
	EEPROM.begin(EEPROM_START_ADDR);
	int addr=0; 
	char eeprom_data_ok[]="FAIL";
	for(uint8_t i=0;i<4;i++) {
		EEPROM.write(addr,eeprom_data_ok[i]);
		addr++;
	}
	for(uint8_t j=0;j<4;j++) {
		for(uint8_t i=0;i<32;i++) {
			EEPROM.write(addr,'\0');
			addr++;
		}
		for(uint8_t i=0;i<64;i++) {
			EEPROM.write(addr,'\0');
			addr++;
		}
	}
	EEPROM.commit();
	_eeprom_data_ok=0;
}

