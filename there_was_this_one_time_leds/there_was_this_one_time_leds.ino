// et up 7 led pins starting at pin 2
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8};
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

void setup() {
  // Initialize each LED pin as an output
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Start with all LEDs off
  }
}

void loop() {
  // Example loop to blink LEDs
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(100);
    digitalWrite(ledPins[i], LOW);
  }
}
