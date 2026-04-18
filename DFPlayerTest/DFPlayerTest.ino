#include <DFRobot_DF1201S.h>

namespace {
constexpr uint32_t kUsbBaud = 115200;
constexpr uint32_t kPlayerBaud = 115200;
constexpr uint8_t kStartupVolume = 20;  // Valid range: 0..30
constexpr uint32_t kInitRetryMs = 1000;
constexpr uint32_t kInitTimeoutMs = 15000;
constexpr uint32_t kPromptDelayMs = 2000;
constexpr uint32_t kStatusPeriodMs = 1000;
constexpr uint16_t kMaxTracks = 64;
}  // namespace

DFRobot_DF1201S player;
uint16_t play_order[kMaxTracks];
String played_names[kMaxTracks];
uint16_t total_files = 0;
uint16_t order_index = 0;
uint16_t played_unique_count = 0;
uint32_t last_status_ms = 0;
bool playback_started = false;
bool playback_complete = false;
bool last_playing_state = false;
bool stop_command_sent = false;

void setLed(bool on) {
  digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
}

void pulseBootPattern() {
  for (int i = 0; i < 3; ++i) {
    setLed(true);
    delay(100);
    setLed(false);
    delay(100);
  }
}

void forcePausePlayback() {
  if (player.pause()) {
    Serial.println("Pause command sent");
  } else {
    Serial.println("Pause command failed");
  }
  delay(150);
}

String normalizeFileName(String name) {
  name.trim();
  String normalized = "";
  for (uint16_t i = 0; i < name.length(); ++i) {
    const char c = name[i];
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
      continue;
    }
    normalized += static_cast<char>(toupper(static_cast<unsigned char>(c)));
  }
  return normalized;
}

bool hasPlayedFileName(const String& normalized_name) {
  for (uint16_t i = 0; i < played_unique_count; ++i) {
    if (played_names[i] == normalized_name) {
      return true;
    }
  }
  return false;
}

void rememberFileName(const String& normalized_name) {
  if (normalized_name.length() == 0 || hasPlayedFileName(normalized_name)) {
    return;
  }
  if (played_unique_count < kMaxTracks) {
    played_names[played_unique_count++] = normalized_name;
  }
}

bool initPlayer() {
  const uint32_t start_ms = millis();

  while (!player.begin(Serial1)) {
    Serial.println("DFPlayer init failed, retrying...");
    setLed(true);
    delay(60);
    setLed(false);
    delay(kInitRetryMs);

    if (millis() - start_ms >= kInitTimeoutMs) {
      return false;
    }
  }

  Serial.println("DFPlayer init OK");
  player.switchFunction(player.MUSIC);
  delay(kPromptDelayMs);
  player.setPrompt(false);
  forcePausePlayback();
  const bool play_mode_ok = player.setPlayMode(player.SINGLE);
  player.setVol(kStartupVolume);
  player.enableAMP();

  Serial.print("Set SINGLE mode: ");
  Serial.println(play_mode_ok ? "OK" : "FAILED");
  Serial.print("Play mode: ");
  Serial.println(player.getPlayMode());
  Serial.print("Volume: ");
  Serial.println(player.getVol());

  return true;
}

void buildPlayOrder() {
  for (uint16_t i = 0; i < total_files; ++i) {
    play_order[i] = i + 1;
  }

  for (int i = total_files - 1; i > 0; --i) {
    const int j = random(i + 1);
    const uint16_t tmp = play_order[i];
    play_order[i] = play_order[j];
    play_order[j] = tmp;
  }

  Serial.print("Random play order: ");
  for (uint16_t i = 0; i < total_files; ++i) {
    Serial.print(play_order[i]);
    if (i + 1 < total_files) {
      Serial.print(", ");
    }
  }
  Serial.println();
}

void logCurrentTrack() {
  Serial.print("Current track number: ");
  Serial.println(player.getCurFileNumber());

  Serial.print("Current filename: ");
  Serial.println(player.getFileName());

  Serial.print("Elapsed / total (s): ");
  Serial.print(player.getCurTime());
  Serial.print(" / ");
  Serial.println(player.getTotalTime());
}

bool startTrack(uint16_t file_number) {
  Serial.print("Starting file number ");
  Serial.println(file_number);

  if (!player.playFileNum(file_number)) {
    Serial.println("playFileNum failed");
    setLed(false);
    return false;
  }

  delay(200);
  const String raw_name = player.getFileName();
  const String normalized_name = normalizeFileName(raw_name);

  Serial.print("Resolved filename: ");
  Serial.println(raw_name);

  if (normalized_name.length() == 0) {
    Serial.println("Filename query returned empty, skipping this slot.");
    forcePausePlayback();
    return false;
  }

  if (hasPlayedFileName(normalized_name)) {
    Serial.println("This filename was already played once, skipping duplicate slot.");
    forcePausePlayback();
    return false;
  }

  rememberFileName(normalized_name);
  playback_started = true;
  last_playing_state = true;
  stop_command_sent = false;
  setLed(true);
  logCurrentTrack();
  return true;
}

bool startNextUniqueTrack() {
  while (order_index < total_files) {
    const uint16_t candidate = play_order[order_index++];
    if (startTrack(candidate)) {
      Serial.print("Unique songs played so far: ");
      Serial.println(played_unique_count);
      return true;
    }
    delay(200);
  }

  Serial.println("No more unique filenames remain. Stopping.");
  playback_complete = true;
  setLed(false);
  return false;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  setLed(false);
  pulseBootPattern();

  Serial.begin(kUsbBaud);
  delay(1500);
  Serial.println();
  Serial.println("=== DFPlayer random-once test starting ===");

  Serial1.begin(kPlayerBaud);
  Serial.println("Serial1 on GP0/GP1 started at 115200");

  if (!initPlayer()) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  total_files = player.getTotalFile();
  Serial.print("Total files reported by DFPlayer: ");
  Serial.println(total_files);

  if (total_files == 0) {
    Serial.println("No playable files found.");
    return;
  }

  if (total_files > kMaxTracks) {
    Serial.print("Clamping file count from ");
    Serial.print(total_files);
    Serial.print(" to ");
    Serial.println(kMaxTracks);
    total_files = kMaxTracks;
  }

  randomSeed(micros());
  buildPlayOrder();
  startNextUniqueTrack();
}

void loop() {
  if (playback_complete || total_files == 0) {
    if (!stop_command_sent) {
      forcePausePlayback();
      stop_command_sent = true;
    }
    setLed(false);
    return;
  }

  if (millis() - last_status_ms >= kStatusPeriodMs) {
    last_status_ms = millis();
    Serial.print("Playing: ");
    Serial.println(player.isPlaying() ? "yes" : "no");
    logCurrentTrack();
  }

  const bool is_playing = player.isPlaying();

  if (playback_started && last_playing_state && !is_playing) {
    Serial.println("Track finished.");
    forcePausePlayback();
    delay(250);
    if (!startNextUniqueTrack()) {
      Serial.println("All unique songs have been played once. Stopping.");
      return;
    }
  }

  last_playing_state = is_playing;
  setLed(is_playing);
}
