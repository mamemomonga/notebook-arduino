# Arduino Every での利用

[Arduino Every](https://store.arduino.cc/usa/nano-every-pack) は ATMega4809をつかったMicroサイズのArduino。UNO,Microより安価。ATMega4809はmegaAVR/0シリーズなので、内部ではUPDIが利用される。

接続されているボードの確認

	$ arduino-cli board list
	Port                            Type              Board Name         FQBN                     Core
	/dev/ttyACM0 Serial Port (USB) Arduino Nano Every arduino:megaavr:nona4809 arduino:megaavr

**/dev/ttyACM0** ポートで **arduino:megaavr:nona4809** というボードで認識されている

コアのインストール

	$ arduino-cli core install arduino:megaavr

コアの一覧

	$ arduino-cli core list
	arduino:megaavr 1.8.6     1.8.6  Arduino megaAVR Boards

スケッチの作成

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

コンパイル

	$ arduino-cli compile --fqbn arduino:megaavr:nona4809 Blink
	Sketch uses 1120 bytes (2%) of program storage space. Maximum is 49152 bytes.
	Global variables use 22 bytes (0%) of dynamic memory, leaving 6122 bytes for local variables. Maximum is 6144 bytes

アップロード

	$ arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:megaavr:nona4809 Blink

`avrdude: jtagmkII_initialize(): Cannot locate "flash" and "boot" memories in description` と表示されるが、正常にアップロードされている。`-v` オプションだとアップロードの詳細が表示される。


