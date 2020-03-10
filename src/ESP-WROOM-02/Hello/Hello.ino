void setup(void) {
    pinMode(4, OUTPUT);
	Serial.begin(115200);
	Serial.println();
	Serial.println("Serial Ready");
}

void loop(void) {
    digitalWrite(4, HIGH);
	Serial.println("Hello");
    delay(500);
    digitalWrite(4, LOW);
    delay(500);
}
