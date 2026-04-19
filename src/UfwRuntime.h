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
};

class UfwRuntime {
 public:
  UfwRuntime(HardwareSerial& playerSerial, const uint8_t* ledPins,
             size_t ledPinCount, int statusLedPin = 10,
             int builtinPin = LED_BUILTIN);

  template <size_t N>
  UfwRuntime(HardwareSerial& playerSerial, const uint8_t (&ledPins)[N],
             int statusLedPin = 10, int builtinPin = LED_BUILTIN)
      : UfwRuntime(playerSerial, ledPins, N, statusLedPin, builtinPin) {}

  bool begin(const char* title,
             const UfwRuntimeConfig& config = UfwRuntimeConfig());

  UfwAudio& audio();
  UfwLed& leds();

 private:
  UfwAudio audio_;
  UfwLed leds_;
};

#endif
