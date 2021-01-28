void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  while(!Serial);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("--- Hello World! ---\r\n");
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
