void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial1.begin(115200);
  while(!Serial1);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Port0");
  Serial1.println("Port1");
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
