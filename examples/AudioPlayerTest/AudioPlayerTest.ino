/*
  AudioPlayerTest

  This is a minimal track-based DFPlayer bench test.
  It uses the same `UfwRuntime` + `UfwTrackPiece` setup style as
  `RememberThisOneTime`, but keeps the authoring surface focused on checking
  audio playback.

  What it shows:
  - how to boot the board with `UfwRuntime`
  - how to define one or more test tracks with `UfwSound`
  - how to let `UfwTrackPiece` handle repeat/stop behavior
  - how to log DFPlayer status while the piece runs

  Edit:
  - `TEST_TRACKS` to choose which DFPlayer tracks to play
  - `REPEAT_TEST_TRACKS` if you want the list to loop automatically
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board is different. ---
const int STATUS_LED_PIN = 10;
const int STATUS_PERIOD_MS = 1000;
const bool REPEAT_TEST_TRACKS = true;

// --- Edit here: choose which tracks to test. ---
UfwSound TEST_TRACKS[] = {
  {"test track", 1},
};

// --- Library objects: you usually do not need to edit below this line. ---
UfwRuntime runtime(Serial1, nullptr, 0, STATUS_LED_PIN);
UfwTrackPiece piece("Audio Player Test", TEST_TRACKS);
uint32_t last_status_ms = 0;

void maybeLogStatus() {
  if (millis() - last_status_ms < STATUS_PERIOD_MS) {
    return;
  }

  last_status_ms = millis();
  Serial.print("Playing: ");
  Serial.println(runtime.audio().isPlaying() ? "yes" : "no");

  Serial.print("Current track number: ");
  Serial.println(runtime.audio().getCurFileNumber());

  Serial.print("Elapsed / total (s): ");
  Serial.print(runtime.audio().getCurTime());
  Serial.print(" / ");
  Serial.println(runtime.audio().getTotalTime());
}

void setup() {
  // `runtime.begin()` handles serial, LED boot setup, and DFPlayer startup.
  if (!runtime.begin("Audio Player Test")) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  Serial.print("Total files reported by DFPlayer: ");
  Serial.println(runtime.audio().getTotalFile());

  piece.begin(runtime.audio(), runtime.leds());
  piece.inOrder();
  if (REPEAT_TEST_TRACKS) {
    piece.repeatForever();
  } else {
    piece.stopWhenDone();
  }
  piece.start();
}

void loop() {
  piece.update();
  maybeLogStatus();
}
