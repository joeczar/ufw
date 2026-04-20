/*
  TwoCuesInOrder

  One step above OneCue: two cues play in the listed order and the piece stops.
  This proves the cue piece advances automatically when one audio ends.

  Edit:
  - CUES            (two {name, track, LED pin} entries)
  - CUE_LED_PINS    must include every LED pin listed in CUES

  You usually do not need to edit anything below the cue list.
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board or wiring is different. ---
const int STATUS_LED_PIN = 10;
const uint8_t CUE_LED_PINS[] = {2, 4};

// --- Edit here: the two cues in play order. ---
UfwCue CUES[] = {
  {"first",  1, 2},
  {"second", 2, 4},
};

// --- Library objects: you usually do not need to edit below this line. ---
UfwRuntime runtime(Serial1, CUE_LED_PINS, STATUS_LED_PIN);
UfwCuePiece piece("Two Cues In Order", CUES);

void setup() {
  if (!runtime.begin("Two Cues In Order")) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  piece.begin(runtime.audio(), runtime.leds());
  piece.inOrder();
  piece.stopWhenDone();
  piece.start();
}

void loop() {
  piece.update();
}
