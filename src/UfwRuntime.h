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

  // LED-only: no DFPlayer. Runs the standard boot sequence (LED setup,
  // optional dev mode, optional boot pulse, serial banner) without
  // requiring DFPlayer hardware. `audio()` is not usable after this
  // constructor; check `hasAudio()` first if you are writing generic code.
  //
  // Requires a global `Serial1` at link time (used as the inactive audio
  // backing). Works on RP2040/Pico, Mega, Leonardo, ESP32, STM32. Boards
  // without `Serial1` (Uno R3, classic Nano, 8-bit AVR) will not compile
  // this constructor.
  UfwRuntime(const uint8_t* ledPins, size_t ledPinCount,
             int externalStatusPin = -1, int builtinPin = LED_BUILTIN);

  template <size_t N>
  UfwRuntime(const uint8_t (&ledPins)[N], int externalStatusPin = -1,
             int builtinPin = LED_BUILTIN)
      : UfwRuntime(ledPins, N, externalStatusPin, builtinPin) {}

  bool begin(const char* title,
             const UfwRuntimeConfig& config = UfwRuntimeConfig());

  UfwAudio& audio();
  UfwLed& leds();
  bool hasAudio() const { return audioEnabled_; }

 private:
  UfwAudio audio_;
  UfwLed leds_;
  bool audioEnabled_;
};

#endif
