#ifndef UFW_RUNTIME_H
#define UFW_RUNTIME_H

#include <Arduino.h>

#include "UfwAudio.h"
#include "UfwLed.h"

struct UfwRuntimeConfig {
  uint32_t usbBaud = 115200;
  uint32_t playerBaud = 115200;
  uint8_t startupVolume = 20;
  uint16_t serialReadyDelayMs = 1500;
  bool pulseBootPattern = true;
  uint8_t bootPulses = 3;
  uint16_t bootOnMs = 100;
  uint16_t bootOffMs = 100;
  bool devMode = false;
};

class UfwRuntime {
 public:
  // Audio only, no phrase/cue LEDs. The board's built-in LED serves as the
  // status indicator (driven only when dev mode is on).
  explicit UfwRuntime(HardwareSerial& playerSerial);

  // Audio + animation LED pins (phrase or cue LEDs). The built-in LED is
  // the status indicator; the optional externalStatusPin lets an existing
  // sketch keep driving an additional LED if its wiring has one.
  UfwRuntime(HardwareSerial& playerSerial, const uint8_t* ledPins,
             size_t ledPinCount, int externalStatusPin = -1,
             int builtinPin = LED_BUILTIN);

  template <size_t N>
  UfwRuntime(HardwareSerial& playerSerial, const uint8_t (&ledPins)[N],
             int externalStatusPin = -1, int builtinPin = LED_BUILTIN)
      : UfwRuntime(playerSerial, ledPins, N, externalStatusPin, builtinPin) {}

  bool begin(const char* title,
             const UfwRuntimeConfig& config = UfwRuntimeConfig());

  UfwAudio& audio();
  UfwLed& leds();

 private:
  UfwAudio audio_;
  UfwLed leds_;
};

#endif
