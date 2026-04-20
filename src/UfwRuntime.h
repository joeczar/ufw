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
  // Audio only: no animation pins, no status LED, no builtin LED.
  // Use for pure-audio sketches that drive no LEDs at all.
  explicit UfwRuntime(HardwareSerial& playerSerial);

  // Audio + status LED only: no animation pins.
  // Use for sketches that need the debug status LED but no phrase/cue LEDs.
  UfwRuntime(HardwareSerial& playerSerial, int statusLedPin,
             int builtinPin = -1);

  // Full: animation LED pins + status LED + optional builtin LED.
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
