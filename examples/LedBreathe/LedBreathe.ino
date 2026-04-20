/*
  LedBreathe

  This example teaches the three breathe helpers:
  - `startBreatheOnce(...)`
  - `startBreatheCount(...)`
  - `startBreathe(...)`

  The first helper does one breathe and stops.
  The second helper does a chosen number of breathes and stops.
  The third helper keeps breathing until your sketch stops it.

  Edit:
  - `LED_PINS` if your LEDs use different GPIO pins
  - `BREATHE_BRIGHTNESS` if you want a dimmer or brighter breathe
  - `BREATHE_COUNT` if you want more or fewer counted breathes
*/

#include <Ufw.h>

const int BREATHE_FRAME_MS = 20;
const int BREATHE_BRIGHTNESS_STEP = 8;
const int BREATHE_BRIGHTNESS = 180;
const int BREATHE_COUNT = 2;
const int LOOP_PREVIEW_MS = 2500;
const int PAUSE_MS = 700;

const uint8_t LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};

UfwLed leds(LED_PINS);

void waitForAnimation() {
  while (leds.isAnimating()) {
    leds.update();
  }
}

void setup() {
  leds.begin();
  leds.setDevMode(true);
  leds.clear();
  leds.pulseBootPattern();
}

void loop() {
  leds.clear();
  leds.setStatus(true);
  leds.startBreatheOnce(BREATHE_FRAME_MS, BREATHE_BRIGHTNESS,
                        BREATHE_BRIGHTNESS_STEP);
  waitForAnimation();
  delay(PAUSE_MS);

  leds.clear();
  leds.setStatus(true);
  leds.startBreatheCount(BREATHE_FRAME_MS, BREATHE_COUNT, BREATHE_BRIGHTNESS,
                         BREATHE_BRIGHTNESS_STEP);
  waitForAnimation();
  delay(PAUSE_MS);

  leds.clear();
  leds.setStatus(true);
  leds.startBreathe(BREATHE_FRAME_MS, BREATHE_BRIGHTNESS,
                    BREATHE_BRIGHTNESS_STEP);

  const uint32_t preview_started_ms = millis();
  while (millis() - preview_started_ms < LOOP_PREVIEW_MS) {
    leds.update();
  }

  leds.stopAnimation();
  delay(PAUSE_MS);
}
