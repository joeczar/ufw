/*
  OneCue

  Smallest possible cue piece.
  One cue = one sound + one LED. When the cue plays, its audio track plays and
  its LED stays lit for the length of that audio. Then the piece stops.

  This is the atom that `IFuckingHateMen` is built from. Everything above this
  rung adds one idea at a time (a second cue, shuffle, repeat).

  Edit:
  - CUES      (the single {name, track, LED pin})
  - CUE_LED_PINS must include every LED pin listed in CUES

  You usually do not need to edit anything below the cue list.
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board or wiring is different. ---
const int STATUS_LED_PIN = 10;

// Every LED pin referenced by any cue must appear in this list.
const uint8_t CUE_LED_PINS[] = {2};

// --- Edit here: the single cue for this piece. ---
// { readable name, DFPlayer track number, LED pin }
UfwCue CUES[] = {
  {"hello", 1, 2},
};

// --- Library objects: you usually do not need to edit below this line. ---
UfwRuntime runtime(Serial1, CUE_LED_PINS, STATUS_LED_PIN);
UfwCuePiece piece("One Cue", CUES);

void setup() {
  if (!runtime.begin("One Cue")) {
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
