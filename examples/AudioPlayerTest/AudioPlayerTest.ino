/*
  AudioPlayerTest

  This is a direct `UfwAudio` example and hardware test.
  Use it when you want to verify the DFPlayer by itself without involving
  LED phrases or piece classes.

  What it shows:
  - how to create a `UfwAudio` object directly
  - how to start the DFPlayer
  - how to play one track
  - how to detect when a track has finished

  Edit:
  - `TEST_TRACK` to choose which DFPlayer track to play
  - `REPEAT_TEST_TRACK` if you want it to replay automatically
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board is different. ---
const int USB_BAUD = 115200;
const int PLAYER_BAUD = 115200;
const int STARTUP_VOLUME = 20;
const int TEST_TRACK = 1;
const int STATUS_PERIOD_MS = 1000;
const int TRACK_GAP_MS = 500;
const bool REPEAT_TEST_TRACK = true;

// --- Library object: using the audio helper on its own. ---
UfwAudio audio(Serial1);
bool audio_ready = false;
bool track_started = false;
bool last_playing_state = false;
uint32_t last_status_ms = 0;

bool startTestTrack() {
  Serial.print("Starting test track ");
  Serial.println(TEST_TRACK);

  if (!audio.playFileNum(TEST_TRACK, &Serial)) {
    return false;
  }

  track_started = true;
  last_playing_state = true;
  return true;
}

void logStatus() {
  Serial.print("Playing: ");
  Serial.println(audio.isPlaying() ? "yes" : "no");

  Serial.print("Current track number: ");
  Serial.println(audio.getCurFileNumber());

  Serial.print("Elapsed / total (s): ");
  Serial.print(audio.getCurTime());
  Serial.print(" / ");
  Serial.println(audio.getTotalTime());
}

void setup() {
  // `setup()` runs once. Start serial first so you can read debug output.
  Serial.begin(USB_BAUD);
  delay(1500);
  Serial.println();
  Serial.println("=== Audio Player Test ===");

  // Start the DFPlayer on Serial1.
  if (!audio.begin(PLAYER_BAUD, Serial, STARTUP_VOLUME)) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }
  audio_ready = true;

  Serial.print("Total files reported by DFPlayer: ");
  Serial.println(audio.getTotalFile());

  startTestTrack();
}

void loop() {
  if (!audio_ready) {
    return;
  }

  // Print a status line every second so you can see what the player is doing.
  if (millis() - last_status_ms >= STATUS_PERIOD_MS) {
    last_status_ms = millis();
    logStatus();
  }

  const bool is_playing = audio.isPlaying();

  // Detect the moment when the test track finishes.
  if (track_started && last_playing_state && !is_playing) {
    Serial.println("Test track finished.");
    audio.pause(&Serial);
    delay(TRACK_GAP_MS);

    if (REPEAT_TEST_TRACK) {
      startTestTrack();
      return;
    }
  }

  last_playing_state = is_playing;
}
