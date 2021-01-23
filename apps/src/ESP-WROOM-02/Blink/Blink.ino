void setup(void) {
    pinMode(4, OUTPUT);
}

void loop(void) {
    digitalWrite(4, HIGH);
    delay(500);
    digitalWrite(4, LOW);
    delay(500);
}
