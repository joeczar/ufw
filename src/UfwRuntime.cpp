#include "UfwRuntime.h"

UfwRuntime::UfwRuntime(HardwareSerial& playerSerial)
    : audio_(playerSerial),
      leds_(nullptr, 0, -1, LED_BUILTIN),
      audioEnabled_(true) {}

UfwRuntime::UfwRuntime(HardwareSerial& playerSerial, const uint8_t* ledPins,
                       size_t ledPinCount, int externalStatusPin,
                       int builtinPin)
    : audio_(playerSerial),
      leds_(ledPins, ledPinCount, externalStatusPin, builtinPin),
      audioEnabled_(true) {}

UfwRuntime::UfwRuntime(const uint8_t* ledPins, size_t ledPinCount,
                       int externalStatusPin, int builtinPin)
    : audio_(Serial1),
      leds_(ledPins, ledPinCount, externalStatusPin, builtinPin),
      audioEnabled_(false) {}

bool UfwRuntime::begin(const char* title, const UfwRuntimeConfig& config) {
  leds_.begin();
  leds_.setDevMode(config.devMode);
  leds_.clear();
  if (config.pulseBootPattern) {
    leds_.pulseBootPattern(config.bootPulses, config.bootOnMs, config.bootOffMs);
  }

  Serial.begin(config.usbBaud);
  delay(config.serialReadyDelayMs);
  Serial.println();
  if (title != nullptr && title[0] != '\0') {
    Serial.print("=== ");
    Serial.print(title);
    Serial.println(" ===");
  }

  if (!audioEnabled_) {
    return true;
  }

  return audio_.begin(config.playerBaud, Serial, config.startupVolume);
}

UfwAudio& UfwRuntime::audio() {
  return audio_;
}

UfwLed& UfwRuntime::leds() {
  return leds_;
}
