# SPI

* SPI通信のサンプル。
* SPIはシフトレジスタ、双方のメモリを交換する形で通信する。
* この例では、32バイト固定長のデータを入れ替えることで双方向通信を行う。

# 使用デバイス

* Master: Arduino Every
* Slave: Arduino UNO

# 接続

それぞれのピンを同じ場所に接続する

内容  | Arduino | AVR | 概要
-----|---------|-----|------
SCK  | D13 | PB5 | 内臓LEDと共有
MISO | D12 | PB4
MOSI | D11 | PB3
SS   | D10 | PB2
