# [Arduino Nano Every](https://store.arduino.cc/usa/nano-every) Serial1

Serial.begin()で使われるのは内臓のUSB-UARTで、Serial1.begin()で使われるのはRX,TXピンである。(AVR:RX1, TX1)

* [Arduino Nano Every](https://store.arduino.cc/usa/nano-every)
* [ピンアウト](https://content.arduino.cc/assets/Pinout-NANOevery_latest.pdf)
* [回路図](https://content.arduino.cc/assets/NANOEveryV3.0_sch.pdf)

このサンプルコードでは外付けUSB-UARTを接続して通信するサンプルである。RX,TXをクロスして接続する。

