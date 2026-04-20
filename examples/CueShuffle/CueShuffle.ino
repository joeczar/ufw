/*
  CueShuffle

  One-line diff from TwoCuesInOrder: `.shuffle()` instead of `.inOrder()`.
  The cues play in a random order each boot, then the piece stops.

  Edit:
  - CUES            ({name, track, LED pin} entries)
  - CUE_LED_PINS    must include every LED pin listed in CUES

  You usually do not need to edit anything below the cue list.
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board or wiring is different. ---
const uint8_t CUE_LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};

// --- Edit here: the cues to shuffle. ---
UfwCue CUES[] = {
  {"one",   1, 2},
  {"two",   2, 4},
  {"three", 3, 5},
  {"four",  4, 6},
  {"five",  5, 7},
  {"six",   6, 8},
  {"seven", 7, 9},
};

// --- Library objects: you usually do not need to edit below this line. ---
UfwRuntime runtime(Serial1, CUE_LED_PINS);
UfwCuePiece piece("Cue Shuffle", CUES);

void setup() {
  if (!runtime.begin("Cue Shuffle")) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  randomSeed(micros());
  piece.begin(runtime.audio(), runtime.leds());
  piece.shuffle();
  piece.stopWhenDone();
  piece.start();
}

void loop() {
  piece.update();
}
