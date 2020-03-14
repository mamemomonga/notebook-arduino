#ifndef _CONFIG_H_
#define _CONFIG_H_

// 初期セットアップ時に使用するWiFi情報
#define WIFISETUP_SSID       "WROOM-SETUP5"
#define WIFISETUP_HOSTNAME   "wroom-setup5"
#define WIFISETUP_IPADDRESS  192,168,4,1

// パスフェーズは8文字以上必要
#define WIFISETUP_PASSPHASE  "wroom-setup-admin"
#define WIFISETUP_CHANNEL    1

#define LED_STATUS 4
#define FORCE_CONFIG_PIN 12

#endif

