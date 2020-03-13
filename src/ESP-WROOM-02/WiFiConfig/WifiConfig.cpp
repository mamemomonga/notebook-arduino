#include "WiFiConfig.h"
#include "WebData.h"

WiFiConfigClass::WiFiConfigClass() {
}

void WiFiConfigClass::begin() {

	delay(100);
	pinMode(12,INPUT_PULLUP);

	while(1) {

		if( digitalRead(FORCE_CONFIG_PIN) == LOW ) {
			_setup_wifi_info();
		}

		if( !storage.load() ) {
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

void WiFiConfigClass::send_error() {
	server->send(200,"text/json","{ \"error\": 1 }");
}

void WiFiConfigClass::send_success() {
	server->send(200,"text/json","{ \"error\": 0 }");
}

void WiFiConfigClass::_setup_wifi_info() {

	// CaptivePortal
	_server_enable=1;

	WiFi.mode(WIFI_STA);

	ESP8266WiFiClass softap;
	ESP8266WebServer server(80);
	DNSServer dnsServer;

	IPAddress apIP(WIFISETUP_IPADDRESS);
	softap.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

	if( softap.softAP( WIFISETUP_SSID )) {
		Serial.println("[SetupWifi] SoftAP Start\r\n");
	}

	if(dnsServer.start(53, "*", apIP)) {
		Serial.println("[SetupWifi] DNS Start\r\n");
	}

	if(MDNS.begin(WIFISETUP_HOSTNAME)) {
		Serial.println("[SetupWifi] mDNS Start\r\n");
	}


	Serial.printf("[SetupWifi] Start\r\n");
	Serial.printf("[SetupWifi] SSID: %s\r\n", WIFISETUP_SSID );

	delay(100);

	IPAddress myIP = softap.softAPIP();
	Serial.print("[SetupWifi] AP IP address: ");
	Serial.println(myIP);


	WebDataClass web_data;

	server.on("/",[&](){
		Serial.println("[SetupWifi] Serve /");
		server.send(200, "text/html",web_data.WiFiSetupIndex());
		return;
	});

	server.on("/api/page",[&](){
		server.send(200, "text/json","{ \"error\": 0,\"page\":\"index\" }");
	});

	server.on("/api/ap/config",[&](){
		Serial.println("[SetupWifi] Serve /api/ap/config");
		if( server.method() != HTTP_POST ) {
			send_error();
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
				Serial.printf("[SetupWifi] Update Entry %d: SSID: %s\r\n",i,storage.WifiConnectionInfo[i].ssid,ssid);
			}
		}
		storage.save();
		server.send(200, "text/json","{ \"error\": 0 }");
		_server_enable=0;
		return;
	});

	server.on("/api/ap/reset",[&](){
		Serial.println("[SetupWifi] Serve /api/ap/reset");
		if( server.method() != HTTP_POST ) {
			send_error();
			return;
		}
		if( server.arg("execute_reset").toInt() == 42 ) {
			storage.clear();
			_server_enable=0;
			send_success();
		} else {
			send_error();
		}
		return;
	});

	server.on("/api/ap/scan",[&](){
		Serial.println("[SetupWifi] Serve /api/ap/scan");

		// WiFiスキャン
		WiFi.disconnect();
		delay(100);
		uint8_t networksFound = WiFi.scanNetworks(false,false);
		Serial.printf("[SetupWifi] %d network(s) found\r\n", networksFound);

		for (int i = 0; i < networksFound; i++) {
			Serial.printf("[SetupWifi]   %d: %s, Ch:%d (%ddBm) %s\r\n",
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
		Serial.println("[SetupWifi] Serve /api/ap/current");

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
		Serial.println("[SetupWifi] Serve 404");
		char redirect[32];
		snprintf_P(redirect,sizeof(redirect),PSTR("http://%d.%d.%d.%d/"),myIP[0],myIP[1],myIP[2],myIP[3]);
		server.sendHeader("Location", redirect, true);
		server.send(301,"text/plain","");
	});

	server.begin();
	Serial.println("[SetupWifi] HTTP server started");

	unsigned long last_millis=0;

	// サーバプロセスとLED点滅
	while(_server_enable) {

		dnsServer.processNextRequest();
		server.handleClient();
		MDNS.update();

		unsigned long current_millis=millis();
		if( ( current_millis - last_millis ) > 50 ) {
			if( digitalRead(LED_STATUS) ) { digitalWrite(LED_STATUS,LOW); } else { digitalWrite(LED_STATUS,HIGH); }
			last_millis=current_millis;
		}
	}

}

uint8_t WiFiConfigClass::_connect_to_wifi() {
	Serial.printf("[SetupWifiConnect] CONNECT TO WIFI\r\n");
	ESP8266WiFiMulti wifiMulti;

	uint8_t ready=0;
	for(uint8_t i=0;i<4;i++) {
		if( storage.WifiConnectionInfo[i].ssid[0]      == '\0' ) { continue; }
		if( storage.WifiConnectionInfo[i].passphase[0] == '\0' ) { continue; }
		ready=1;
		Serial.print("[SetupWifiConnect] ADD SSID:"); Serial.println( storage.WifiConnectionInfo[i].ssid );
		wifiMulti.addAP(storage.WifiConnectionInfo[i].ssid, storage.WifiConnectionInfo[i].passphase);
	}
	if(!ready) {
		Serial.printf("[SetupWifiConnect] CONFIG FAILED\r\n");
		return 1;
	}

	Serial.printf("[SetupWifiConnect] Connecting...\r\n");
	while(wifiMulti.run() != WL_CONNECTED) {
		if( digitalRead(FORCE_CONFIG_PIN) == LOW ) { return 1; }
		digitalWrite(LED_STATUS,LOW);
		delay(200);
		digitalWrite(LED_STATUS,HIGH);
		delay(200);
	}

	IPAddress ip = WiFi.localIP();
	uint8_t * mac = WiFi.BSSID();
	Serial.printf("[SetupWifiConnect] Connected!\r\n");
	Serial.printf("[SetupWifiConnect]   SSID: %s\r\n",WiFi.SSID().c_str());
	Serial.printf("[SetupWifiConnect]   Channel: %d\r\n", WiFi.channel());
	Serial.printf("[SetupWifiConnect]   MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	Serial.printf("[SetupWifiConnect]   IP: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
	return 0;

}
