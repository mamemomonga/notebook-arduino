# MiniCore

現在検証中

https://github.com/MCUdude/MiniCore

## 配線

* [MiniCore - Pinout](https://github.com/MCUdude/MiniCore/blob/master/README.md#pinout)
* [MiniCore - Minimal setup](https://github.com/MCUdude/MiniCore/blob/master/README.md#minimal-setup)

## 設定

以下 の board_manager の項目に書き加える

* Linux: $HOME/.arduino15/arduino-cli.yaml 
* MacOS: $HOME/Library/Arduino15/arduino-cli.yaml

arduino-cli.yaml

	$ vim $HOME/.arduino15/arduino-cli.yaml
	board_manager:
	  additional_urls:
	  - https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json

コア一覧の更新

	$ arduino-cli core update-index

コア検索

	$ arduino-cli core search MiniCore
    ID           Version Name
    MiniCore:avr 2.0.9   MiniCore

コアインストール

	$ arduino-cli core install MiniCore:avr
	$ arduino-cli core list
    ID              Installed Latest Name
    MiniCore:avr    2.0.9     2.0.9  MiniCore

    $ arduino-cli board listall | grep 'MiniCore'
    ATmega168                        MiniCore:avr:168
    ATmega328                        MiniCore:avr:328
    ATmega48                         MiniCore:avr:48
    ATmega8                          MiniCore:avr:8
    ATmega88                         MiniCore:avr:88

ブートローダの書込

どうもわたしの環境ではブートローダはうまくいかなかった。

 * arduino-cli burn-bootloader -b MiniCore:avr:328:variant=modelPB -P avrispmkii
 * arduino-cli upload -P avrispmkii --fqbn MiniCore:avr:328:variant=modelPB Blink
