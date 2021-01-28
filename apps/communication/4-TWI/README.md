# 2台のArduinoでi2c通信

i2c | Arduino | ATmega328P
----|---------|--------------
SCL | A5, D19 | PC5 
SDA | A4, D18 | PC4

* それぞれのピンを双方のArduinoに接続する
* それぞれのピンと+5Vとの間に10KΩ抵抗を挟む(プルアップ抵抗)
