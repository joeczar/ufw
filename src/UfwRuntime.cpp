#include "UfwRuntime.h"

UfwRuntime::UfwRuntime(HardwareSerial& playerSerial, const uint8_t* ledPins,
                       size_t ledPinCount, int statusLedPin, int builtinPin)
    : audio_(playerSerial),
      leds_(ledPins, ledPinCount, statusLedPin, builtinPin) {}

bool UfwRuntime::begin(const char* title, const UfwRuntimeConfig& config) {
  leds_.begin();
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

  return audio_.begin(config.playerBaud, Serial, config.startupVolume);
}

UfwAudio& UfwRuntime::audio() {
  return audio_;
}

UfwLed& UfwRuntime::leds() {
  return leds_;
}
