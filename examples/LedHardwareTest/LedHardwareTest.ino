/*
  LedHardwareTest

  This is a direct `UfwLed` example and hardware test.
  Use it when you want to verify your LED wiring without involving audio.

  What it shows:
  - how to create a `UfwLed` object directly
  - how to turn one LED pin on at a time
  - how to use the built-in fill sweep animation
  - how to use a moving sweep with a max-lit window
  - how to use the built-in fade sweep animation with a brightness trail
  - how to use the built-in comet animation
  - how to use the built-in breathe animation in once, count, and loop modes

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
const int BREATHE_FRAME_MS = 20;
const int BREATHE_BRIGHTNESS_STEP = 8;
const int BREATHE_BRIGHTNESS = 180;
const int BREATHE_COUNT = 2;
const int BREATHE_LOOP_PREVIEW_MS = 2500;
const int GAP_MS = 250;
const int MAX_LIT_WINDOW = 3;

// These are the phrase LEDs we want to test, in order.
const uint8_t LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};
// Brightness profile runs from the newest "head" LED to the oldest "tail" LED.
const uint8_t FADE_TRAIL[] = {255, 120, 40};
const uint8_t COMET_TRAIL[] = {255, 180, 80};

// --- Library object: using the LED helper on its own. ---
UfwLed leds(LED_PINS, STATUS_LED_PIN);

enum TestMode {
  TEST_WALK_PINS,
  TEST_FULL_SWEEP,
  TEST_WINDOW_SWEEP,
  TEST_FADE_SWEEP,
  TEST_COMET,
  TEST_BREATHE_ONCE,
  TEST_BREATHE_COUNT,
  TEST_BREATHE_LOOP,
};

TestMode current_test = TEST_WALK_PINS;
size_t walk_index = 0;
uint32_t last_walk_ms = 0;
uint32_t test_started_ms = 0;
bool test_started = false;

void walkPinsOneByOne() {
  // Turn each pin on by itself so you can confirm every LED lights.
  if (!test_started) {
    test_started = true;
    walk_index = 0;
    last_walk_ms = millis();
    leds.clear();
    leds.setStatus(true);
    leds.setPin(LED_PINS[walk_index], true);
    return;
  }

  if (millis() - last_walk_ms < STEP_HOLD_MS) {
    return;
  }

  last_walk_ms = millis();
  ++walk_index;
  if (walk_index >= ufwCountOf(LED_PINS)) {
    leds.clear();
    test_started = false;
    current_test = TEST_FULL_SWEEP;
    return;
  }

  leds.clear();
  leds.setStatus(true);
  leds.setPin(LED_PINS[walk_index], true);
}

void runFullSweepTest() {
  // This uses the full-bank sweep: as many LEDs may be lit as there are pins.
  if (!test_started) {
    test_started = true;
    test_started_ms = millis();
    leds.setStatus(true);
    leds.startFillSweep(SWEEP_STEP_MS);
    return;
  }

  if (leds.isAnimating()) {
    return;
  }

  leds.clear();
  test_started = false;
  current_test = TEST_WINDOW_SWEEP;
}

void runWindowSweepTest() {
  // This sweep still crosses all LEDs, but never lights more than MAX_LIT_WINDOW.
  if (!test_started) {
    test_started = true;
    test_started_ms = millis();
    leds.setStatus(true);
    leds.startFillSweep(SWEEP_STEP_MS, MAX_LIT_WINDOW);
    return;
  }

  if (leds.isAnimating()) {
    return;
  }

  leds.clear();
  test_started = false;
  current_test = TEST_FADE_SWEEP;
}

void runFadeSweepTest() {
  // This sweep fades each window position toward the profile above, then
  // advances once those LEDs have reached their new target brightness.
  if (!test_started) {
    test_started = true;
    test_started_ms = millis();
    leds.setStatus(true);
    leds.startFadeSweep(FADE_FRAME_MS, MAX_LIT_WINDOW, FADE_TRAIL,
                        FADE_BRIGHTNESS_STEP);
    return;
  }

  if (leds.isAnimating()) {
    return;
  }

  leds.clear();
  test_started = false;
  current_test = TEST_COMET;
}

void runCometTest() {
  // A comet is a moving bright head with a softer fading tail behind it.
  if (!test_started) {
    test_started = true;
    test_started_ms = millis();
    leds.setStatus(true);
    leds.startComet(FADE_FRAME_MS, MAX_LIT_WINDOW, COMET_TRAIL,
                    FADE_BRIGHTNESS_STEP);
    return;
  }

  if (leds.isAnimating()) {
    return;
  }

  leds.clear();
  test_started = false;
  current_test = TEST_BREATHE_ONCE;
}

void runBreatheOnceTest() {
  // `startBreatheOnce()` does one full in-and-out breath, then stops by itself.
  if (!test_started) {
    test_started = true;
    test_started_ms = millis();
    leds.setStatus(true);
    leds.startBreatheOnce(BREATHE_FRAME_MS, BREATHE_BRIGHTNESS,
                          BREATHE_BRIGHTNESS_STEP);
    return;
  }

  if (leds.isAnimating()) {
    return;
  }

  leds.clear();
  test_started = false;
  current_test = TEST_BREATHE_COUNT;
}

void runBreatheCountTest() {
  // `startBreatheCount()` runs the requested number of complete breaths.
  if (!test_started) {
    test_started = true;
    test_started_ms = millis();
    leds.setStatus(true);
    leds.startBreatheCount(BREATHE_FRAME_MS, BREATHE_COUNT, BREATHE_BRIGHTNESS,
                           BREATHE_BRIGHTNESS_STEP);
    return;
  }

  if (leds.isAnimating()) {
    return;
  }

  leds.clear();
  test_started = false;
  current_test = TEST_BREATHE_LOOP;
}

void runBreatheLoopTest() {
  // `startBreathe()` loops forever until something else stops it.
  if (!test_started) {
    test_started = true;
    test_started_ms = millis();
    leds.setStatus(true);
    leds.startBreathe(BREATHE_FRAME_MS, BREATHE_BRIGHTNESS,
                      BREATHE_BRIGHTNESS_STEP);
    return;
  }

  if (millis() - test_started_ms < BREATHE_LOOP_PREVIEW_MS) {
    return;
  }

  leds.stopAnimation();
  leds.clear();
  test_started = false;
  current_test = TEST_WALK_PINS;
}

void setup() {
  // `setup()` runs once. Initialize the LED helper and show a short boot blink.
  leds.begin();
  leds.clear();
  leds.pulseBootPattern();
}

void loop() {
  // `update()` keeps any active LED animation moving without blocking.
  leds.update();

  // `loop()` cycles through the LED tests forever:
  // one pin at a time, the sweep family, comet, then breathe variations.
  switch (current_test) {
    case TEST_WALK_PINS:
      walkPinsOneByOne();
      break;
    case TEST_FULL_SWEEP:
      runFullSweepTest();
      break;
    case TEST_WINDOW_SWEEP:
      runWindowSweepTest();
      break;
    case TEST_FADE_SWEEP:
      runFadeSweepTest();
      break;
    case TEST_COMET:
      runCometTest();
      break;
    case TEST_BREATHE_ONCE:
      runBreatheOnceTest();
      break;
    case TEST_BREATHE_COUNT:
      runBreatheCountTest();
      break;
    case TEST_BREATHE_LOOP:
      runBreatheLoopTest();
      break;
  }

  delay(5);
}
