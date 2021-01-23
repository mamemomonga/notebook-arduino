// ---------------------
// SPI Master
// ---------------------
// SCK  D13(PB5) (内臓LEDと共有)
// MISO D12(PB4)
// MOSI D11(PB3)
// SS   D10(PB2)

#include <SPI.h>
#include <string.h>

#define PIN_SS 10

uint16_t counter = 0;

void setup() {
    // シリアルの有効化
    Serial.begin(115200);
    while(!Serial);

    pinMode(PIN_SS,OUTPUT);
    SPI.begin();                          // SPI Master有効化
    SPI.setBitOrder(MSBFIRST);            // MSBFIRST
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    SPI.setDataMode(SPI_MODE2);           // CPOL=1, CPHA=0

    Serial.print("[SPI Master]\r\n");
}

void loop() {
    uint8_t sendData[32];
    uint8_t recvData[32];

    // 送信データ
    String str="Hello from MASTER! (" + String(counter,DEC)+")";
    str.toCharArray(sendData,32);
    counter++;

    for(uint8_t i=0; i<32; i++) {
        digitalWrite(PIN_SS,LOW); delayMicroseconds(1);
        recvData[i]=SPI.transfer(sendData[i]);
        digitalWrite(PIN_SS,HIGH); delayMicroseconds(1);
    }

    Serial.print("Send:    "); Serial.print((char*)sendData); Serial.print("\r\n");
    Serial.print("Recieve: "); Serial.print((char*)recvData); Serial.print("\r\n");
    delay(250);
}