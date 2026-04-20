/*
  LedSetPin

  This example teaches one `UfwLed` idea:
  use `leds.setPin(...)` to turn on a specific LED.

  `LED_PINS` is a list of pin numbers.
  `LED_PINS[0]` means "the first pin number in the list".
  `LED_PINS[1]` means "the second pin number in the list".
  `LED_PINS[2]` means "the third pin number in the list".

  Edit:
  - `LED_PINS` if your LEDs use different GPIO pins
  - `STATUS_LED_PIN` if your status LED is on a different pin
  - `HOLD_MS` if you want each LED to stay on for longer or shorter
*/

#include <Ufw.h>

const int STATUS_LED_PIN = 10;
const int HOLD_MS = 700;

const uint8_t LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};

UfwLed leds(LED_PINS, STATUS_LED_PIN);

void setup() {
  leds.begin();
  leds.clear();
  leds.pulseBootPattern();
}

void loop() {
  leds.clear();
  leds.setStatus(true);
  const uint8_t FIRST_LED_PIN = LED_PINS[0];
  leds.setPin(FIRST_LED_PIN, true);
  delay(HOLD_MS);

  leds.clear();
  leds.setStatus(true);
  const uint8_t SECOND_LED_PIN = LED_PINS[1];
  leds.setPin(SECOND_LED_PIN, true);
  delay(HOLD_MS);

  leds.clear();
  leds.setStatus(true);
  const uint8_t THIRD_LED_PIN = LED_PINS[2];
  leds.setPin(THIRD_LED_PIN, true);
  delay(HOLD_MS);
}
