/*
  AudioPlayOne

  Smallest possible audio example. No LEDs at all.
  It boots the DFPlayer, plays track 1 once, and stops.

  Edit:
  - TRACK_NUMBER if your target file is not track 1

  You usually do not need to edit anything below the TRACK_NUMBER line.
*/

#include <Ufw.h>

// --- Edit here: which track should play on boot. ---
const uint16_t TRACK_NUMBER = 1;

// --- Library objects: you usually do not need to edit below this line. ---
// Audio-only runtime: no animation LEDs, no status LED, no builtin LED.
UfwRuntime runtime(Serial1);

void setup() {
  if (!runtime.begin("Audio Play One")) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  runtime.audio().playFileNum(TRACK_NUMBER, &Serial);
}

void loop() {
}
