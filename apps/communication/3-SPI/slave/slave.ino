// ---------------------
// SPI Slave
// ---------------------
// Arduino UNO(avr) 専用
// Arduino Every(megaavr)などでは動作しません

// SCK  D13(PB5) (内臓LEDと共有)
// MISO D12(PB4)
// MOSI D11(PB3)
// SS   D10(PB2)

#include <SPI.h>
#include <string.h>

uint8_t  sendData[33]="";
uint8_t  recvData[32]="";
uint8_t  spiPos=0;
uint16_t counter=0;
uint8_t  update=0;

// 送信するデータの構築
void setSendData() {
    String str="Hello from SLAVE! (" + String(counter,DEC)+")";
    counter++;
    spiPos = 0;

    cli();
    str.toCharArray(sendData,32);
    SPDR = sendData[0];
    sei();
}

void setup() {
    // シリアルの有効化
    Serial.begin(115200);
    while(!Serial);

    Serial.print("[SPI Slave]\r\n");

    // デバッグ用LED
    pinMode(2,OUTPUT);

    // SPI Slaveの設定
    SPCR |= bit(SPE);           // SPI有効化
    pinMode(10,INPUT);          // SS
    pinMode(MISO,OUTPUT);       // MISO
    SPI.setBitOrder(MSBFIRST);  // MSBFIRST
    SPI.setDataMode(SPI_MODE2); // CPOL=1, CPHA=0
    SPI.attachInterrupt();

    // 相手からデータが来る前にSPDRの1つめをセットしておく
    setSendData();
}

// SPI割込
ISR(SPI_STC_vect){
    recvData[spiPos]=SPDR;
    spiPos++;
    SPDR = sendData[spiPos];
    if(spiPos < 32) {
        return;
    }
    setSendData();
    update = 1;
}

void loop() {

    digitalWrite(2,HIGH);
    if(update) {
        Serial.print("Recieve: "); Serial.print((char*)recvData); Serial.print("\r\n");
        Serial.print("Send:    "); Serial.print((char*)sendData); Serial.print("\r\n");
        update = 0;
    }
    delay(500);

    delay(500);
    digitalWrite(2,LOW);
    delay(500);
}
