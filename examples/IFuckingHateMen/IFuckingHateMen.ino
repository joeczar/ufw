/*
  IFuckingHateMen

  This is a cue-based piece.
  Each cue links one sound to one LED, and the piece shuffles whole cue units.
  That means the sound and its matching LED stay together when the order changes.

  Edit:
  - the cue list
  - the piece behavior settings in `setup()`

  You usually do not need to edit anything below the cue list.
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board or wiring is different. ---
// These are the LED pins this piece is allowed to light.
const uint8_t CUE_LED_PINS[] = {2, 4, 5, 6, 7, 8};

// --- Edit here: change the sound-and-light pairs in the piece. ---
// Each cue is:
// { readable name, DFPlayer track number, LED pin }
//
// This is different from a track piece: here each sound is tightly linked
// to one LED, and the piece shuffles those linked pairs together.
UfwCue CUES[] = {
  {"i", 1, 2},
  {"fuh", 2, 4},
  {"kuh", 3, 5},
  {"ing", 4, 6},
  {"hate", 5, 7},
  {"men", 6, 8},
};

// --- Library objects: you usually do not need to edit below this line. ---
// `runtime` owns the board boot setup, DFPlayer startup, and LED setup.
UfwRuntime runtime(Serial1, CUE_LED_PINS);
// `piece` owns the cue order, repeat behavior, and cue-to-cue transitions.
UfwCuePiece piece("I Fucking Hate Men", CUES);

void setup() {
  // `runtime.begin()` handles the repetitive board setup for you:
  // LED boot setup, serial setup, and DFPlayer startup.
  if (!runtime.begin("I Fucking Hate Men")) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  // --- Piece behavior: this is the main authoring surface. ---
  // Shuffle the whole cue list and repeat forever.
  randomSeed(micros());
  piece.begin(runtime.audio(), runtime.leds());
  piece.shuffle();
  piece.repeatForever();
  piece.start();
}

void loop() {
  // `piece.update()` is the engine that keeps the cue piece running.
  // It waits for one cue to finish, then starts the next cue automatically.
  // Most pieces built from this example will keep `loop()` this simple.
  piece.update();
}
