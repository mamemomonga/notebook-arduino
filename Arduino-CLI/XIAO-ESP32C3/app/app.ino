#include "config.h"

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
	pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
	Serial.println("HIGH");
	digitalWrite(LED_BUILTIN, HIGH);
	delay(1000);
	Serial.println("LOW");
	digitalWrite(LED_BUILTIN, LOW);
	delay(1000);
}
