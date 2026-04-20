/*
  AudioStatusLed

  One step above AudioPlayOne: a status LED reflects playback.
  While the track is playing, the status LED is on. When it stops, the LED is off.

  This is the one example that turns dev mode ON, because the status LED is a
  developer-facing signal. Finished pieces leave dev mode off so installations
  run dark.

  Edit:
  - TRACK_NUMBER
  - STATUS_LED_PIN if your wiring is different

  You usually do not need to edit anything below the TRACK_NUMBER line.
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board or wiring is different. ---
const int STATUS_LED_PIN = 10;

// Rate-limit DFPlayer polls. isPlaying() is a UART round-trip, so polling it
// every loop iteration floods the serial link and can corrupt playback commands.
const uint32_t STATUS_PERIOD_MS = 250;

// --- Edit here: which track should play on boot. ---
const uint16_t TRACK_NUMBER = 1;

// --- Library objects: you usually do not need to edit below this line. ---
UfwRuntime runtime(Serial1, STATUS_LED_PIN);
uint32_t last_status_ms = 0;

void setup() {
  UfwRuntimeConfig config;
  config.devMode = true;

  if (!runtime.begin("Audio Status LED", config)) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  runtime.audio().playFileNum(TRACK_NUMBER, &Serial);
}

void loop() {
  if (millis() - last_status_ms < STATUS_PERIOD_MS) {
    return;
  }
  last_status_ms = millis();
  runtime.leds().setStatus(runtime.audio().isPlaying());
}
