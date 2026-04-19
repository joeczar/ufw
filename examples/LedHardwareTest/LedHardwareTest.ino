/*
  LedHardwareTest

  This is a direct `UfwLed` example and hardware test.
  Use it when you want to verify your LED wiring without involving audio.

  What it shows:
  - how to create a `UfwLed` object directly
  - how to turn one LED pin on at a time
  - how to use the built-in fill sweep animation
  - how to use the built-in fade sweep animation

  Edit:
  - `LED_PINS` if your LED wiring uses different GPIO pins
  - `STATUS_LED_PIN` if your status LED is on a different pin
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your wiring is different. ---
const int STATUS_LED_PIN = 10;
const int STEP_HOLD_MS = 500;
const int SWEEP_STEP_MS = 120;
const int FADE_FRAME_MS = 16;
const int FADE_BRIGHTNESS_STEP = 16;
const int GAP_MS = 250;

// These are the phrase LEDs we want to test, in order.
const uint8_t LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};

// --- Library object: using the LED helper on its own. ---
UfwLed leds(LED_PINS, STATUS_LED_PIN);

void walkPinsOneByOne() {
  // Turn each pin on by itself so you can confirm every LED lights.
  for (size_t i = 0; i < ufwCountOf(LED_PINS); ++i) {
    leds.clear();
    leds.setStatus(true);
    leds.setPin(LED_PINS[i], true);
    delay(STEP_HOLD_MS);
  }

  leds.clear();
  delay(GAP_MS);
}

void runSweepTest() {
  // Now test the built-in sweep animation that pieces can use.
  leds.startFillSweep(SWEEP_STEP_MS);
  while (leds.isAnimating()) {
    leds.update();
    delay(5);
  }

  leds.clear();
  delay(GAP_MS);
}

void runFadeSweepTest() {
  // This test fades each LED up and back down before moving to the next one.
  leds.startFadeSweep(FADE_FRAME_MS, FADE_BRIGHTNESS_STEP);
  while (leds.isAnimating()) {
    leds.update();
    delay(5);
  }

  leds.clear();
  delay(GAP_MS);
}

void setup() {
  // `setup()` runs once. Initialize the LED helper and show a short boot blink.
  leds.begin();
  leds.clear();
  leds.pulseBootPattern();
}

void loop() {
  // `loop()` keeps running the LED tests forever:
  // first each pin by itself, then a hard sweep, then a fade sweep.
  walkPinsOneByOne();
  runSweepTest();
  runFadeSweepTest();
}
