/*
  LedHardwareTest

  This is the full `UfwLed` demo and hardware test.
  It exercises every configured LED pin and every built-in LED helper:
  - `setPin(...)`
  - `startFillSweep(...)`
  - `startFillSweep(...)` with `maxLit`
  - `startFadeSweep(...)`
  - `startComet(...)`
  - `startBreatheOnce(...)`
  - `startBreatheCount(...)`
  - `startBreathe(...)`

  Use the smaller LED examples when you want to learn one concept at a time.
  Use this sketch when you want to verify that the whole LED helper surface
  works on your hardware.

  Edit:
  - `LED_PINS` if your LEDs use different GPIO pins
  - `STATUS_LED_PIN` if your status LED is on a different pin
*/

#include <Ufw.h>

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
const int STAGE_GAP_MS = 300;
const int MAX_LIT_WINDOW = 3;

const uint8_t LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};
const uint8_t FADE_TRAIL[] = {255, 120, 40};
const uint8_t COMET_TRAIL[] = {255, 180, 80};

UfwLed leds(LED_PINS, STATUS_LED_PIN);

void waitForAnimation() {
  while (leds.isAnimating()) {
    leds.update();
  }
}

void pauseBetweenStages() {
  leds.clear();
  delay(STAGE_GAP_MS);
}

void checkEachLedPin() {
  leds.setStatus(true);
  for (size_t i = 0; i < ufwCountOf(LED_PINS); ++i) {
    leds.clear();
    leds.setStatus(true);
    leds.setPin(LED_PINS[i], true);
    delay(STEP_HOLD_MS);
  }
}

void runFullSweep() {
  leds.clear();
  leds.setStatus(true);
  leds.startFillSweep(SWEEP_STEP_MS);
  waitForAnimation();
}

void runLimitedSweep() {
  leds.clear();
  leds.setStatus(true);
  leds.startFillSweep(SWEEP_STEP_MS, MAX_LIT_WINDOW);
  waitForAnimation();
}

void runFadeSweep() {
  leds.clear();
  leds.setStatus(true);
  leds.startFadeSweep(FADE_FRAME_MS, MAX_LIT_WINDOW, FADE_TRAIL,
                      FADE_BRIGHTNESS_STEP);
  waitForAnimation();
}

void runComet() {
  leds.clear();
  leds.setStatus(true);
  leds.startComet(FADE_FRAME_MS, MAX_LIT_WINDOW, COMET_TRAIL,
                  FADE_BRIGHTNESS_STEP);
  waitForAnimation();
}

void runBreatheOnce() {
  leds.clear();
  leds.setStatus(true);
  leds.startBreatheOnce(BREATHE_FRAME_MS, BREATHE_BRIGHTNESS,
                        BREATHE_BRIGHTNESS_STEP);
  waitForAnimation();
}

void runBreatheCount() {
  leds.clear();
  leds.setStatus(true);
  leds.startBreatheCount(BREATHE_FRAME_MS, BREATHE_COUNT, BREATHE_BRIGHTNESS,
                         BREATHE_BRIGHTNESS_STEP);
  waitForAnimation();
}

void previewLoopingBreathe() {
  leds.clear();
  leds.setStatus(true);
  leds.startBreathe(BREATHE_FRAME_MS, BREATHE_BRIGHTNESS,
                    BREATHE_BRIGHTNESS_STEP);

  const uint32_t preview_started_ms = millis();
  while (millis() - preview_started_ms < BREATHE_LOOP_PREVIEW_MS) {
    leds.update();
  }

  leds.stopAnimation();
}

void setup() {
  leds.begin();
  leds.clear();
  leds.pulseBootPattern();
}

void loop() {
  checkEachLedPin();
  pauseBetweenStages();

  runFullSweep();
  pauseBetweenStages();

  runLimitedSweep();
  pauseBetweenStages();

  runFadeSweep();
  pauseBetweenStages();

  runComet();
  pauseBetweenStages();

  runBreatheOnce();
  pauseBetweenStages();

  runBreatheCount();
  pauseBetweenStages();

  previewLoopingBreathe();
  pauseBetweenStages();
}
