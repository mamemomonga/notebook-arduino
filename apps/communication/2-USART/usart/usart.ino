
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    while(!Serial);
}

void loop() {
    Serial.print("--- Hello World! ---\r\n");

    for(uint8_t i=0; i<50; i++){
        while(Serial.available()) {
            digitalWrite(LED_BUILTIN, HIGH);
            int val = Serial.read();
            Serial.print("[INPUT] ");
            Serial.print("CODE:0x"); Serial.print(val,HEX); Serial.print(" ");
            Serial.print("ASCII:"); Serial.write(val); Serial.print("\r\n");
            digitalWrite(LED_BUILTIN, LOW);
        }
        delay(100);
    }
}
