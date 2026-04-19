/*
  RememberThisOneTime

  This is a track-based piece.
  Each named sound is a longer poem track on the DFPlayer.
  When a track starts, the LED phrase for "I re mem ber this one time" runs once.

  Edit:
  - the poem track list
  - the LED phrase steps
  - the piece behavior settings in `setup()`

  You usually do not need to edit anything below the piece setup section.
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board or wiring is different. ---
const int STATUS_LED_PIN = 10;
const int PHRASE_STEP_DELAY_MS = 120;

// These are the LED pins available to the phrase player.
// The phrase below will step through these pins once when a poem starts.
const uint8_t PHRASE_LED_PINS[] = {2, 4, 5, 6, 7, 8, 9};

// --- Edit here: choose the poem tracks for this piece. ---
// The left value is a readable name for you.
// The right value is the actual DFPlayer track number.
UfwSound POEMS[] = {
  {"poem1", 1},
  {"poem2", 2},
  {"poem3", 3},
};

// --- Edit here: choose the LED phrase that plays once at track start. ---
// Each step is:
// { LED pin, how long that LED stays on in milliseconds }
//
// This phrase matches the syllables of "I re mem ber this one time".
UfwPhraseStep REMEMBER_STEPS[] = {
  {2, PHRASE_STEP_DELAY_MS},
  {4, PHRASE_STEP_DELAY_MS},
  {5, PHRASE_STEP_DELAY_MS},
  {6, PHRASE_STEP_DELAY_MS},
  {7, PHRASE_STEP_DELAY_MS},
  {8, PHRASE_STEP_DELAY_MS},
  {9, PHRASE_STEP_DELAY_MS},
};

// This named phrase packages the LED steps so the piece can trigger it.
UfwPhrase REMEMBER_PHRASE = {
  "remember phrase",
  REMEMBER_STEPS,
  ufwCountOf(REMEMBER_STEPS),
};

// --- Library objects: you usually do not need to edit below this line. ---
// `runtime` owns the board boot setup, DFPlayer startup, and LED setup.
UfwRuntime runtime(Serial1, PHRASE_LED_PINS, STATUS_LED_PIN);
// `piece` owns the playback order and the start/stop behavior for the whole work.
UfwTrackPiece piece("Remember This One Time", POEMS, &REMEMBER_PHRASE);

void setup() {
  // `runtime.begin()` handles the repetitive board setup for you:
  // LED boot setup, serial setup, and DFPlayer startup.
  if (!runtime.begin("Remember This One Time")) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  // --- Piece behavior: this is the main authoring surface. ---
  // Shuffle the poem tracks, stop after all poems have played once, then start.
  randomSeed(micros());
  piece.begin(runtime.audio(), runtime.leds());
  piece.shuffle();
  piece.stopWhenDone();
  piece.start();
}

void loop() {
  // `piece.update()` is the engine that keeps the piece moving forward.
  // It watches for track endings and starts the next track when needed.
  // Most pieces built from this example will keep `loop()` this simple.
  piece.update();
}
