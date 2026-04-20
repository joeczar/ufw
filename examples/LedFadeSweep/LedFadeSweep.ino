/*
  LedFadeSweep

  This example teaches `startFadeSweep(...)`.

  `FADE_TRAIL` is a list of brightness values.
  The first number is the bright front of the moving light.
  The later numbers are dimmer LEDs behind it.

  Edit:
  - `LED_PINS` if your LEDs use different GPIO pins
  - `STATUS_LED_PIN` if your status LED is on a different pin
  - `FADE_TRAIL` if you want a different brightness shape
  - `MAX_LIT_WINDOW` if you want the bright moving group to be bigger or smaller
*/

#include <Ufw.h>

const int STATUS_LED_PIN = 10;
const int FADE_FRAME_MS = 16;
const int FADE_BRIGHTNESS_STEP = 16;
const int MAX_LIT_WINDOW = 3;
const int PAUSE_MS = 700;

const uint8_t LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};
const uint8_t FADE_TRAIL[] = {255, 120, 40};

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
  leds.startFadeSweep(FADE_FRAME_MS, MAX_LIT_WINDOW, FADE_TRAIL,
                      FADE_BRIGHTNESS_STEP);
  waitForAnimation();
  delay(PAUSE_MS);
}
