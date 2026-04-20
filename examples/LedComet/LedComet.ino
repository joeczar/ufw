/*
  LedComet

  This example teaches `startComet(...)`.

  `COMET_TRAIL` is a list of brightness values.
  The first number is the bright front of the comet.
  The later numbers are the dimmer LEDs behind it.

  Edit:
  - `LED_PINS` if your LEDs use different GPIO pins
  - `STATUS_LED_PIN` if your status LED is on a different pin
  - `COMET_TRAIL` if you want a different comet shape
  - `MAX_LIT_WINDOW` if you want the comet to be bigger or smaller
*/

#include <Ufw.h>

const int STATUS_LED_PIN = 10;
const int FRAME_MS = 16;
const int BRIGHTNESS_STEP = 16;
const int MAX_LIT_WINDOW = 3;
const int PAUSE_MS = 700;

const uint8_t LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};
const uint8_t COMET_TRAIL[] = {255, 180, 80};

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
  leds.startComet(FRAME_MS, MAX_LIT_WINDOW, COMET_TRAIL, BRIGHTNESS_STEP);
  waitForAnimation();
  delay(PAUSE_MS);
}
