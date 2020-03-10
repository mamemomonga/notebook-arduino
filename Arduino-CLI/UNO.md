# Arduino UNO での利用

Arduinoの代表。Atmel(Microchip) ATmega328P(8ビットマイコン 32kBメモリ)を搭載する。

接続されているArduinoの確認

	$ arduino-cli board list
	Port         Type              Board Name  FQBN            Core
	/dev/ttyACM0 Serial Port (USB) Arduino Uno arduino:avr:uno arduino:avr
	/dev/ttyS0   Serial Port       Unknown
	/dev/ttyS1   Serial Port       Unknown
	/dev/ttyS2   Serial Port       Unknown
	/dev/ttyS3   Serial Port       Unknown

コアのインストール FQBNを指定する

	$ arduino-cli core install arduino:avr
	$ arduino-cli core list
	ID          Installed Latest Name
	arduino:avr 1.8.2     1.8.2  Arduino AVR Boards

## スケッチを書く

	$ arduino-cli sketch new UnoBlink
	$ cat > UnoBlink/UnoBlink.ino << 'EOS'
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

スケッチのコンパイル

	$ arduino-cli compile --fqbn arduino:avr:uno UnoBlink
	Sketch uses 924 bytes (2%) of program storage space. Maximum is 32256 bytes.
	Global variables use 9 bytes (0%) of dynamic memory, leaving 2039 bytes for local variables. Maximum is 2048 bytes.

スケッチのアップロード

	$ arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno UnoBlink


