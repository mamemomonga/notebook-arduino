# Arduino CLI

* [Arduino Pro/Arduino CLI](https://www.arduino.cc/pro/cli)
* [Arduino CLI](https://github.com/arduino/arduino-cli)
* [Arduino CLI Documentation](https://arduino.github.io/arduino-cli/)
* [Welcome to ESP8266 Arduino Core’s documentation!](https://arduino-esp8266.readthedocs.io/en/latest/)

# ボードごとの利用例

* [Arduino UNOでの利用](./UNO.md)
* [Arduino Everyでの利用](./Every.md)
* [Arduino Pro互換](./ProMini.md)
* [ESP-WROOM-02 モジュールでの利用](./ESP-WROOM-02.md)

# セットアップ

bash

	$ curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
	$ echo 'export PATH=$HOME/bin:$PATH' >> $HOME/.bashrc
	$ exec $SHELL

macOS Homebrew

	$ brew update
	$ brew install arduino-cli

バージョンの確認

	$ arduino-cli version
	;arduino-cli  Version: 0.23.0 Commit: 899dc91b Date: 2022-06-06T14:23:44Z
	
configの初期化

	$ arduino-cli config init

# 各種Arduino Coreをインストールする

### [MiniCore](https://github.com/MCUdude/MiniCore)
ATmega328, ATmega168, ATmega88, ATmega48, ATmega8

### [megaCoreX](https://github.com/MCUdude/MegaCoreX)
megaAVR0系、ATmega4809, ATmega4808, ATmega3209, ATmega3208, ATmega1609, ATmega1608, ATmega809, ATmega808

### [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)
tinyAVR0,1,2系、ATtiny1606, ATtiny1604, ATtiny412 など

### [ESP8266](https://github.com/arduino/esp8266)
ESP8266系、ESP-WROOM-02など

### [ESP32](https://github.com/espressif/arduino-esp32)
ESP32系

インストール

	sh -eux << 'EOS'
	arduino-cli config add board_manager.additional_urls 'https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json'
	arduino-cli config add board_manager.additional_urls 'https://mcudude.github.io/MegaCoreX/package_MCUdude_MegaCoreX_index.json'
	arduino-cli config add board_manager.additional_urls 'http://drazzy.com/package_drazzy.com_index.json'
	arduino-cli config add board_manager.additional_urls 'https://arduino.esp8266.com/stable/package_esp8266com_index.json'
	arduino-cli core search
	arduino-cli core install arduino:avr
	arduino-cli core install arduino:megaavr
	arduino-cli core install MiniCore:avr
	arduino-cli core install MegaCoreX:megaavr
	arduino-cli core install megaTinyCore:megaavr
	arduino-cli core install esp8266:esp8266
	arduino-cli board list
	arduino-cli board listall
	EOS

ESP32(aarch64非対応)

	arduino-cli config add board_manager.additional_urls 'https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json'
	arduino-cli core update-index
	arduino-cli core install esp32:esp32
	
## (Linux) 一般ユーザでのアップロードでPermission Deniedの場合

### シリアルデバイス

	$ ls -al /dev/ttyACM0
	crw-rw---- 1 root dialout 166, 0 Mar 11 03:58 /dev/ttyACM0
	$ sudo usermod -aG dialout $USER

再起動または再ログインが必要

### USBデバイス(AVRISPMK2など)

	$ echo "SUBSYSTEM==\"usb\", MODE=\"0660\", GROUP=\"$(id -gn)\"" | sudo tee /etc/udev/rules.d/00-usb-permissions.rules
	$ sudo udevadm control --reload-rules
	
# 基本的な使い方

実行の確認

	$ arduino-cli help core

configの作成

	$ arduino-cli config init

コア一覧の更新

	$ arduino-cli core update-index

インストールされているボードリストの表示

	$ arduino-cli board listall
	
インストールされているコアの表示

	$ arduino-cli core list
