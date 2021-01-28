# Arduino Promini(互換品)での利用

Arduino ProMiniはUSB-シリアルインターフェイスを搭載していない ATmega328Pボード。個人的にはあまりおすすめではない。

ただ、ここの方法は素のATmega328Pに水晶とブートローダを組み合わせたものでも動作するはずです(本来だったらArduino Duemilanove互換になるのかな)。

コントロールできるLEDを搭載していない場合は、D13(PB5,SCK)に1KΩの抵抗経由でLEDを接続する。

接続されているボードとポートの確認、ここでは **/dev/cu.usbserial-1001** に接続されている。汎用USARTモジュールを使用しているのでボードは判別できない。

    $ arduino-cli board list

    Port                            Type              Board Name FQBN Core
    /dev/cu.Bluetooth-Incoming-Port Serial Port       Unknown
    /dev/cu.usbserial-1001          Serial Port (USB) Unknown
    /dev/cu.usbserial-1002          Serial Port (USB) Unknown

コアリストの確認

    $ arduino-cli core list
    ID              Installed Latest Name
    arduino:avr     1.8.3     1.8.3  Arduino AVR Boards
    arduino:megaavr 1.8.7     1.8.7  Arduino megaAVR Boards
    esp8266:esp8266 2.7.4     2.7.4  esp8266

arduino:avr がなければ導入する

    $ arduino-cli core install arduino:avr

ProMiniのFQBNを確認する

    $ arduino-cli board listall | grep 'Pro Mini'
    Arduino Pro or Pro Mini          arduino:avr:pro

Lチカのコードを作成、LED_BUILTINはD13(PB5,SCK)

    $ arduino-cli sketch new Blink

    $ cat > Blink/Blink.ino << 'EOS'
    void setup() {
        pinMode(LED_BUILTIN, OUTPUT);
    }
    void loop() {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
    }
    EOS

ビルド

    $ arduino-cli compile --fqbn arduino:avr:pro Blink

アップロード

    $ arduino-cli upload -p /dev/cu.usbserial-1001 --fqbn arduino:avr:pro Blink
