#include "MainApp.h"

MainAppClass::MainAppClass(){}

void MainAppClass::setup() {
	Serial.println("[MainApp] Setup"); 
	while(true) {
		if( digitalRead(FORCE_CONFIG_PIN) == LOW ) {
			wifi_setup();
		}
		if( !storage.load() ) {
			wifi_setup();
		}
		if( wifi_connect() ) {
			break;
		}
		wifi_setup();
	}
}

void MainAppClass::wifi_setup() {

	// CaptivePortal
	uint8_t _server_enable=1;

	WiFi.mode(WIFI_STA);

	ESP8266WiFiClass softap;
	ESP8266WebServer server(80);
	DNSServer dnsServer;

	IPAddress apIP(WIFISETUP_IPADDRESS);
	softap.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

	if( softap.softAP( WIFISETUP_SSID )) {
		Serial.println(F("[WiFiSetup] SoftAP Start"));
	}

	if(dnsServer.start(53, "*", apIP)) {
		Serial.println(F("[WiFiSetup] DNS Start"));
	}

//	if(MDNS.begin(WIFISETUP_HOSTNAME)) {
//		Serial.println(F("[WiFiSetup] mDNS Start"));
//	}

	Serial.printf_P(PSTR("[WiFiSetup] SSID: %s\r\n"), WIFISETUP_SSID );

	IPAddress myIP = softap.softAPIP();
	Serial.printf_P(PSTR("[WiFiSetup] AP IP: %d.%d.%d.%d\r\n"), myIP[0], myIP[1], myIP[2], myIP[3]);

	server.on("/",[&](){
		Serial.println(F("[WiFiSetup] Serve /"));
		server.send(200, "text/html",WebDataWiFiSetupIndex());
		return;
	});

	server.on("/api/page",[&](){
		Serial.println(F("[WiFiSetup] Serve /api/page"));
		server.send(200, "text/json","{ \"error\": 0,\"page\":\"index\" }");
	});

	server.on("/api/ap/config",[&](){
		Serial.println(F("[WiFiSetup] Serve /api/ap/config"));
		if( server.method() != HTTP_POST ) {
			server.send(200,"text/json","{ \"error\": 1 }");
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
				strcpy(storage.WifiConnectionInfo[i].ssid,      ssid);
				strcpy(storage.WifiConnectionInfo[i].passphase, passphase);
				Serial.printf("[WifiSetup] Update Entry %d: SSID: %s\r\n",i,storage.WifiConnectionInfo[i].ssid,ssid);
			}
		}
		storage.save();
		server.send(200, "text/json","{ \"error\": 0 }");
		_server_enable=0;
		return;
	});

	server.on("/api/ap/reset",[&](){
		Serial.println(F("[WiFiSetup] Serve /api/ap/reset"));
		if( server.method() != HTTP_POST ) {
			server.send(200,"text/json","{ \"error\": 1 }");
			return;
		}
		if( server.arg("execute_reset").toInt() == 42 ) {
			storage.clear();
			_server_enable=0;
			server.send(200,"text/json","{ \"error\": 0 }");
		} else {
			server.send(200,"text/json","{ \"error\": 1 }");
		}
		return;
	});

	server.on("/api/ap/scan",[&](){
		Serial.println(F("[WiFiSetup] Serve /api/ap/scan"));

		// WiFiスキャン
		WiFi.disconnect();
		delay(100);
		uint8_t networksFound = WiFi.scanNetworks(false,false);
		Serial.printf_P(PSTR("[WiFiSetup] %d network(s) found\r\n"), networksFound);

		for (int i = 0; i < networksFound; i++) {
			Serial.printf_P(PSTR("[WiFiSetup]   %d: %s, Ch:%d (%ddBm) %s\r\n"),
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
		Serial.println(F("[WiFiSetup] Serve /api/ap/current"));

		storage.load();

		String buf="{ \"result\": 0, \"data\": [";
		for (int i = 0; i < 4; i++) {
			char b[255];
			snprintf_P(b,sizeof(b),PSTR("{\"ssid\":\"%s\"}"),storage.WifiConnectionInfo[i].ssid);
			buf=buf+b;
			if(i == 3) { buf=buf+"]}";} else { buf=buf+","; }
		}
		server.send(200, "text/json",buf);
	});

	server.onNotFound([&]() {
		Serial.println("[WiFiSetup] Serve 404");
		char redirect[32];
		snprintf_P(redirect,sizeof(redirect),PSTR("http://%d.%d.%d.%d/"),myIP[0],myIP[1],myIP[2],myIP[3]);
		server.sendHeader("Location", redirect, true);
		server.send(301,"text/plain","");
	});

	server.begin();
	Serial.println(F("[WiFiSetup] HTTP server started"));

	unsigned long last_millis=0;

	// サーバプロセスとLED点滅
	while(_server_enable) {

		dnsServer.processNextRequest();
		server.handleClient();
//		MDNS.update();

		unsigned long current_millis=millis();
		if( ( current_millis - last_millis ) > 50 ) {
			if( digitalRead(LED_STATUS) ) { digitalWrite(LED_STATUS,LOW); } else { digitalWrite(LED_STATUS,HIGH); }
			last_millis=current_millis;
		}
	}

}

uint8_t MainAppClass::wifi_connect() {
	Serial.printf_P(PSTR("[ConnectWiFi] begin\r\n"));

	ESP8266WiFiMulti wifiMulti;

	uint8_t ready=0;
	for(uint8_t i=0;i<4;i++) {
		if( storage.WifiConnectionInfo[i].ssid[0]      == '\0' ) { continue; }
		if( storage.WifiConnectionInfo[i].passphase[0] == '\0' ) { continue; }
		ready=1;
		Serial.print(F("[ConnectWiFi] ADD SSID:")); Serial.println( storage.WifiConnectionInfo[i].ssid );
		wifiMulti.addAP(storage.WifiConnectionInfo[i].ssid, storage.WifiConnectionInfo[i].passphase);
	}
	if(!ready) {
		Serial.printf_P(PSTR("[ConnectWiFi] CONFIG FAILED\r\n"));
		return 0;
	}

	Serial.printf_P(PSTR("[ConnectWiFi] Connecting...\r\n"));
	while(wifiMulti.run() != WL_CONNECTED) {
		if( digitalRead(FORCE_CONFIG_PIN) == LOW ) { return 1; }
		digitalWrite(LED_STATUS,LOW);
		delay(200);
		digitalWrite(LED_STATUS,HIGH);
		delay(200);
	}

	IPAddress ip = WiFi.localIP();
	uint8_t * mac = WiFi.BSSID();
	Serial.printf("[ConnectWiFi] Connected!\r\n");
	Serial.printf("[ConnectWiFi]   SSID: %s\r\n",WiFi.SSID().c_str());
	Serial.printf("[ConnectWiFi]   Channel: %d\r\n", WiFi.channel());
	Serial.printf("[ConnectWiFi]   MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	Serial.printf("[ConnectWiFi]   IP: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
	return 1;
}
