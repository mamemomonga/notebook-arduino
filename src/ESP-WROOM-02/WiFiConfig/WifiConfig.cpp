#include "WiFiConfig.h"
#include "htmldata.h"

WiFiConfigClass::WiFiConfigClass() {
	for(uint8_t i=0;i<4;i++) {
		strcpy(wci[i].ssid,"");
		strcpy(wci[i].passphase,"");
	}
}

void WiFiConfigClass::begin() {

	delay(1000);
	pinMode(12,INPUT_PULLUP);

	while(1) {

		if( digitalRead(FORCE_CONFIG_PIN) == LOW ) {
			_setup_wifi_info();
		}

		if(_read_config_eeprom()) { // 0:成功
			// 設定開始
			_setup_wifi_info();
		}

		// 接続開始
		if(_connect_to_wifi()) { // 0:成功
			// 接続キャンセルでEEPROM削除
			_clear_eeprom();

		} else {
			// 接続成功
			break;

		}
	}

	delay(1000);
}

void WiFiConfigClass::_setup_wifi_info() {
	// CaptivePortal
	_server_enable=1;

	ESP8266WiFiClass softap;
	ESP8266WebServer server(80);

	DNSServer dnsServer;

	Serial.printf(" -- WIFI SETUP WEB --\r\n");
	Serial.printf("Configuring access point...\r\n");
	Serial.printf("SSID: %s\r\n", WIFISETUP_SSID );

	IPAddress apIP(WIFISETUP_IPADDRESS);
	softap.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	softap.softAP( WIFISETUP_SSID );

	delay(500);

	IPAddress myIP = softap.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);

	dnsServer.start(53, "*", apIP);
	Serial.println("DNS Server started");

	server.on("/",[&](){
		if( server.method() == HTTP_POST) {
			for(uint8_t i=1; i<=4; i++) {
				char ssidname[6]; snprintf(ssidname,6,"ssid%d",i);
				char passname[11]; snprintf(passname,11,"passphase%d",i);
				server.arg(ssidname).toCharArray(wci[i-1].ssid,31);
				server.arg(passname).toCharArray(wci[i-1].passphase,63);
			}
			_write_config_eeprom();
			server.send(200, "text/html", HTMLDATA_WIFISETUP_INDEX_POST_HTML);
			_server_enable=0;

		} else {
			server.send(200, "text/html",HTMLDATA_WIFISETUP_INDEX_HTML);
		}
	});

	char myIPStr[24];
	snprintf(myIPStr,24,"%d.%d.%d.%d",myIP[0],myIP[1],myIP[2],myIP[3]);

	server.onNotFound([&]() {
		server.send(200, "text/html",
			String()+"<html><head><meta http-equiv=\"refresh\" content=\"0; URL=http://"+myIPStr+"/\"></head></body>"
		);
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
		delay(100);
		digitalWrite(LED_STATUS,HIGH);
		delay(100);
	}

	IPAddress ip = WiFi.localIP();
	uint8_t * mac = WiFi.BSSID();
	Serial.printf("Connected!\r\n");
//	Serial.printf("  SSID: %s\r\n", WiFi.SSID());
	Serial.printf("  IP: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
	Serial.printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	Serial.printf("  Channel: %d\n", WiFi.channel());

	return 0;

}

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
		strcpy(wci[0].ssid,""); strcpy(wci[0].passphase,"");
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
	for(uint8_t i=0;i<4;i++) {
		EEPROM.write(addr,0);
		addr++;
	}
	EEPROM.commit();
	_eeprom_data_ok=0;
}

