# Arduinoのメモ帳

基本的に [Arduino Makefile](https://github.com/sudar/Arduino-Makefile) を使ったArduinoの利用メモです。AVRについては[AVRのメモ帳](https://github.com/mamemomonga/notebook-avr)もご覧下さい。

# 資料

* [Arduino IDE](https://www.arduino.cc/en/Main/Software)
* [A Makefile for Arduino Sketches](https://0xdec.gitlab.io/avr-makefile-generator/)


# Debianへのセットアップ

インストール

	$ sudo apt install arduino-mk

## パス

概要 | パス
-----|-----
Arduino サンプル | /usr/share/arduino/examples
Arduino Makefile サンプル | /usr/share/doc/arduino-mk/examples
Arduino ライブラリ | /usr/share/arduino/libraries

ビルド

	$ make

アップロード

	$ make upload

# 点滅

* [Arduino Uno LED点滅](./uno/Blink)
