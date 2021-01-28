#include <Wire.h>

#define I2C_ADDR 0x10

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  while(!Serial);

  Wire.begin(I2C_ADDR);
  Serial.print("Start Slave: 0x");
  Serial.print(I2C_ADDR,HEX);
  Serial.print("\r\n");

  // イベントハンドラ
  Wire.onRequest(requestEvent);
  Wire.onReceive(recieveEvent);
}

// 受信
void recieveEvent() {
  Serial.print("SlaveRecieve: [");
  while(Wire.available() > 1) Wire.read(); // 中途半端なデータは捨てる
  uint16_t ct = Wire.read();
  Serial.print(ct,DEC);
  Serial.print("]\r\n");
}

// 送信
void requestEvent() {
  char c[12] = "HELLO WORLD"; // 11+1=12byte
  Wire.write(c);
  Serial.print("SlaveSend: [");
  Serial.print(c);
  Serial.print("]\r\n");
}

// メインループ
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}