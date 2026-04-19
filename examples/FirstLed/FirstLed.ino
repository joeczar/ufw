/*
  FirstLed

  This is the smallest possible Ufw example.
  It does not use audio or piece classes yet. It only proves that one LED pin
  can blink on your board.

  Edit:
  - `LED_PIN` if your LED is wired to a different GPIO pin
  - `BLINK_MS` if you want it to blink faster or slower
*/

// --- Hardware settings: change these if your wiring is different. ---
const int LED_PIN = 2;
const int BLINK_MS = 1000;

void setup() {
  // `setup()` runs once when the board starts.
  // Here we tell the board that this pin should drive an LED.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // `loop()` runs forever.
  // This example is intentionally simple: turn the LED on, wait, turn it off, wait.
  digitalWrite(LED_PIN, HIGH);
  delay(BLINK_MS);
  digitalWrite(LED_PIN, LOW);
  delay(BLINK_MS);
}
