# Arduinoのメモ帳

基本的に [Arduino Makefile](https://github.com/sudar/Arduino-Makefile) を使ったArduinoの利用メモです。AVRについては[AVRのメモ帳](https://github.com/mamemomonga/notebook-avr)もご覧下さい。

# 資料

* [Arduino IDE](https://www.arduino.cc/en/Main/Software)
* [Language Reference](https://www.arduino.cc/reference/en/)
* [言語リファレンス](http://www.musashinodenpa.com/arduino/ref/)
* [Arduino 日本語リファレンス](http://www.musashinodenpa.com/arduino/ref/)
* [Arduino Makefile](https://github.com/sudar/Arduino-Makefile)

# ATmega328P のArduino化

![ピンアサイン](https://raw.githubusercontent.com/mamemomonga/notebook-avr/master/images/ATmega328P-3.png)

[Optiboot](https://github.com/Optiboot/optiboot)を書き込むことで、AVRをArduinoとして使えます。ブートローダは
/usr/share/arduino/hardware/arduino/bootloaders/atmega にもあるようです。Arduino用のboard.txtとセットの[ATmegaBareDuino](https://github.com/mamemomonga/ATmegaBareDuino)もあります。

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
