/*
  LedFillSweep

  This example teaches two closely related `UfwLed` ideas:
  - `startFillSweep(...)` starts a sweep across the LEDs
  - `maxLit` limits how many LEDs can be on at one time

  `UfwLed` animations are non-blocking.
  That means the animation starts when you call `startFillSweep(...)`,
  and then you keep it moving by calling `leds.update()`.

  Edit:
  - `LED_PINS` if your LEDs use different GPIO pins
  - `STATUS_LED_PIN` if your status LED is on a different pin
  - `SWEEP_STEP_MS` if you want the sweep to move faster or slower
  - `MAX_LIT_WINDOW` if you want more or fewer LEDs on at once
*/

#include <Ufw.h>

const int STATUS_LED_PIN = 10;
const int SWEEP_STEP_MS = 120;
const int PAUSE_MS = 500;
const int MAX_LIT_WINDOW = 3;

const uint8_t LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};

UfwLed leds(LED_PINS, STATUS_LED_PIN);

void waitForAnimation() {
  while (leds.isAnimating()) {
    leds.update();
  }
}

void setup() {
  leds.begin();
  leds.clear();
  leds.pulseBootPattern();
}

void loop() {
  leds.clear();
  leds.setStatus(true);
  leds.startFillSweep(SWEEP_STEP_MS);
  waitForAnimation();
  delay(PAUSE_MS);

  leds.clear();
  leds.setStatus(true);
  leds.startFillSweep(SWEEP_STEP_MS, MAX_LIT_WINDOW);
  waitForAnimation();
  delay(PAUSE_MS);
}
