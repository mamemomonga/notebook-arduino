#include "MainApp.h"

ESP8266WebServer server(80);
ESP8266WiFiClass softap;
DNSServer        dnsServer;
ESP8266WiFiMulti wifiMulti;

MainAppClass::MainAppClass(){
	disable_wifi=false;
}

void MainAppClass::hard_reset() {
	Serial.println("[MainApp] HARD RESET!"); 
	delay(500);
	// 13ピンをRSTにつないでハードリセットに使う
	pinMode(13,OUTPUT);
	digitalWrite(13,LOW);
	// これは正しく動かない
	ESP.reset();
}

void MainAppClass::setup() {
	if(disable_wifi) { return; }
	Serial.println("[MainApp] Init"); 
	sta=0;

	WiFi.mode(WIFI_AP_STA);
	setup_webserver();
	setup_ap();

	if (!MDNS.begin(WIFISETUP_HOSTNAME)) {
		Serial.println("[MainApp] Error setting up MDNS responder!");
	}
	Serial.println("[MainApp] mDNS responder started");
	MDNS.addService("http", "tcp", 80);

	if(storage.load()) {
		sta=1;
	}
	tmpString="";
}

void MainAppClass::handles1() {
	if(disable_wifi) { return; }
	dnsServer.processNextRequest();
	server.handleClient();
	MDNS.update();
}

void MainAppClass::handles2() {
	if(disable_wifi) { return; }
	sta_connect();
}

void MainAppClass::spiffs_ls() {
	Serial.println("SPIFFS LS");
	char cwdName[2];
	strcpy(cwdName,"/");
	Dir dir=SPIFFS.openDir(cwdName);
	while(dir.next()) {
		String fn, fs;
		fn = dir.fileName();
		fn.remove(0, 1);
		fs = String(dir.fileSize());
		Serial.println("<" + fn + "> size=" + fs);
	}
}


void MainAppClass::setup_webserver() {

	Serial.println(F("[WebServer] ----------------------------"));
	SPIFFS.begin();
	spiffs_ls();

	// ----
	server.on("/",[&](){
		Serial.println(F("[WebServer] Serve /"));
		// server.send(200, "text/html",WebDataWiFiSetupIndex());
		File file = SPIFFS.open("/index.html", "r");
		server.streamFile(file, "text/html");
		file.close();
	});

	// ----
	server.on("/api/page",[&](){
		Serial.println(F("[WebServer] Serve /api/page"));
		server.send(200, "text/json","{ \"error\": 0,\"page\":\"index\" }");
	});

	// ----
	server.on("/api/ap/info",[&](){
		char aips[17];
		char sips[17];
		IPAddress api = softap.softAPIP();
		IPAddress sip = WiFi.localIP();
		snprintf(aips,17,"%d.%d.%d.%d",api[0],api[1],api[2],api[3]);
		snprintf(sips,17,"%d.%d.%d.%d",sip[0],sip[1],sip[2],sip[3]);

		char b[255];
		snprintf_P(b, 255, PSTR(R"({"error":0,"hostname":"%s","ap":{"ip":"%s","ssid":"%s"},"sta":{"ip":"%s","ssid":"%s"}})"), WIFISETUP_HOSTNAME, aips, WIFISETUP_SSID, sips, WiFi.SSID().c_str());
		server.send(200, "text/json", String(b));
	});

	// ----
	server.on("/api/ap/config",[&](){
		Serial.println(F("[WebServer] Serve /api/ap/config"));
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
				Serial.printf("[WebServer] Update Entry %d: SSID: %s\r\n",i,storage.WifiConnectionInfo[i].ssid,ssid);
			}
		}
		storage.save();
		server.send(200, "text/json","{ \"error\": 0 }");

		sta=1;
		hard_reset();
		return;
	});

	// ----
	server.on("/api/ap/reset",[&](){
		Serial.println(F("[WebServer] Serve /api/ap/reset"));
		if( server.method() != HTTP_POST ) {
			server.send(200,"text/json","{ \"error\": 1 }");
			return;
		}
		if( server.arg("execute_reset").toInt() == 42 ) {
			storage.clear();
			server.send(200,"text/json","{ \"error\": 0 }");
		} else {
			server.send(200,"text/json","{ \"error\": 1 }");
		}
		hard_reset();
		return;
	});

	// ----
	server.on("/api/ap/scan",[&](){
		// POSTで取得開始
		if( server.method() == HTTP_POST ) {
			Serial.println(F("[WebServer] Serve GET /api/ap/scan"));
			server.send(200,"text/json","{ \"error\": 0 }");

			WiFi.disconnect();
			delay(100);
			uint8_t networksFound = WiFi.scanNetworks(false,false);
			Serial.printf_P(PSTR("[WebServer] %d network(s) found\r\n"), networksFound);
			for (int i = 0; i < networksFound; i++) {
				Serial.printf_P(PSTR("[WebServer]   %d: %s, Ch:%d (%ddBm) %s\r\n"),
					i + 1,
					WiFi.SSID(i).c_str(),
					WiFi.channel(i),
					WiFi.RSSI(i),
					WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : ""
				);
			}
			tmpString="{ \"result\": 0, \"data\": [";
			for (int i = 0; i < networksFound; i++) {
				// Openネットワークは表示しない
				if( WiFi.encryptionType(i) == ENC_TYPE_NONE ) { continue; }
				char b[255];
				snprintf_P(b,sizeof(b),PSTR("{\"ssid\":\"%s\",\"channel\":%d,\"rssi\":%d}"),
					WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i));
				tmpString=tmpString+b;
				if(i == (networksFound-1)) { tmpString=tmpString+"]}"; } else { tmpString=tmpString+","; }
			}
		// GETで結果返答
		} else if( server.method() == HTTP_GET ) {
			server.send(200, "text/json",tmpString);
		} else {
			server.send(200,"text/json","{ \"error\": 1 }");
		}
	});

	// ----
	server.on("/api/ap/list",[&](){
		Serial.println(F("[WebServer] Serve /api/ap/current"));

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

	// ----
	server.onNotFound([&]() {
		Serial.println("[WebServer] Serve 404");
		char redirect[32];
		snprintf_P(redirect,32,PSTR("http://%s/"),server.hostHeader().c_str());
		server.sendHeader("Location", redirect, true);
		server.send(301,"text/plain","");
	});

	server.begin();
	Serial.println(F("[WebServer] HTTP server started"));
}

void MainAppClass::setup_ap() {
	IPAddress apIP(WIFISETUP_IPADDRESS);
	softap.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

	if( softap.softAP( WIFISETUP_SSID, WIFISETUP_PASSPHASE, WIFISETUP_CHANNEL)) {
		Serial.println(F("[SetupAP] AP Start"));
	}

	if(dnsServer.start(53, "*", apIP)) {
		Serial.println(F("[SetupAP] DNS Start"));
	}

	IPAddress ip  = softap.softAPIP();
	Serial.printf("[SetupAP]   SSID: %s\r\n", WIFISETUP_SSID);
	Serial.printf("[SetupAP]   Channel: %d\r\n", WIFISETUP_CHANNEL);
	Serial.printf("[SetupAP]   IP: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
}

void MainAppClass::sta_connect() {
	if( sta==0 ) { return; }

	if(wifiMulti.run() == WL_CONNECTED) {
		if(sta == 2) {
			IPAddress ip = WiFi.localIP();
			uint8_t * mac = WiFi.BSSID();
			Serial.printf("[ConnectSTA]   SSID: %s\r\n",WiFi.SSID().c_str());
			Serial.printf("[ConnectSTA]   Channel: %d\r\n", WiFi.channel());
			Serial.printf("[ConnectSTA]   MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			Serial.printf("[ConnectSTA]   IP: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
			sta = 3;
		}
		return;
	}

	if(sta == 1) {
		for(uint8_t i=0;i<4;i++) {
			if( storage.WifiConnectionInfo[i].ssid[0]      == '\0' ) { continue; }
			if( storage.WifiConnectionInfo[i].passphase[0] == '\0' ) { continue; }
			Serial.print(F("[SetupSTA] Add SSID:")); Serial.println( storage.WifiConnectionInfo[i].ssid );
			wifiMulti.addAP(storage.WifiConnectionInfo[i].ssid, storage.WifiConnectionInfo[i].passphase);
			sta=2;
		}
	}

}
