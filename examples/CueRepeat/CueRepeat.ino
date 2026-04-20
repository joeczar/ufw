/*
  CueRepeat

  One-line diff from TwoCuesInOrder: `.repeatForever()` instead of
  `.stopWhenDone()`. The two cues play in order and loop forever.

  Combine `.shuffle()` + `.repeatForever()` to get the `IFuckingHateMen` shape.

  Edit:
  - CUES            (two {name, track, LED pin} entries)
  - CUE_LED_PINS    must include every LED pin listed in CUES

  You usually do not need to edit anything below the cue list.
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board or wiring is different. ---
const int STATUS_LED_PIN = 10;
const uint8_t CUE_LED_PINS[] = {2, 4};

// --- Edit here: the cues to repeat forever. ---
UfwCue CUES[] = {
  {"first",  1, 2},
  {"second", 2, 4},
};

// --- Library objects: you usually do not need to edit below this line. ---
UfwRuntime runtime(Serial1, CUE_LED_PINS, STATUS_LED_PIN);
UfwCuePiece piece("Cue Repeat", CUES);

void setup() {
  if (!runtime.begin("Cue Repeat")) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  piece.begin(runtime.audio(), runtime.leds());
  piece.inOrder();
  piece.repeatForever();
  piece.start();
}

void loop() {
  piece.update();
}
