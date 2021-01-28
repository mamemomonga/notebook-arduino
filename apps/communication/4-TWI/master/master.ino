#include <Wire.h>

#define I2C_ADDR 0x10

uint16_t counter = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  while (!Serial);

  Serial.print("Start Master Target: 0x");
  Serial.print(I2C_ADDR,HEX);
  Serial.print("\r\n");

  Wire.begin();
}

// i2cスキャナー
void scan() {
  Serial.println("--- START SCAN ---");
  digitalWrite(LED_BUILTIN, HIGH);
  int nDevices = 0;
  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("FOUND: 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("");

      ++nDevices;
    } else if (error == 4) {
      Serial.print("ERROR: 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  Serial.println("--- END SCAN ---");
  digitalWrite(LED_BUILTIN, LOW);
}

// 送信
void send() {
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.print("MasterSend: [");
  Serial.print(counter,DEC);
  Serial.print("]\r\n");

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(counter); // 2バイト
  Wire.endTransmission();

  counter++;
  digitalWrite(LED_BUILTIN, LOW);
}

// 受信
void recieve() {
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.print("MasterRecieve: [");
  Wire.requestFrom(I2C_ADDR,11); // "HELLO WORLD"
  char c[12]; uint8_t cp=0;
  while(Wire.available()) { // 1バイトずつ読込
    c[cp++] = Wire.read();
  }
  c[cp]='\0';
  Serial.print(c);
  Serial.print("]\r\n");

  digitalWrite(LED_BUILTIN, LOW);
}

// メインループ
void loop() {
    // scan();
    recieve();
    delay(500);
    send();
    delay(1000);
}