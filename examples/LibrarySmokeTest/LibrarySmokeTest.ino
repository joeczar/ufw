#include <Ufw.h>

const int USB_BAUD = 115200;
const int PLAYER_BAUD = 115200;
const int STARTUP_VOLUME = 20;
const int ANIMATION_STEP_DELAY_MS = 120;
const int STATUS_PERIOD_MS = 1000;
const int SWEEP_PERIOD_MS = 3000;

const uint8_t LED_PINS[] = {2, 4, 5, 6, 7, 8};
const size_t LED_PIN_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

UfwAudio audio(Serial1);
UfwLed leds(LED_PINS, LED_PIN_COUNT);
bool audio_ready = false;
uint32_t last_status_ms = 0;
uint32_t last_sweep_ms = 0;

void setup() {
  leds.begin();
  // Smoke test is a dev-time sketch: show the status LED and boot pulse.
  leds.setDevMode(true);
  leds.pulseBootPattern();

  Serial.begin(USB_BAUD);
  delay(1500);
  Serial.println();
  Serial.println("=== Library smoke test ===");

  if (!audio.begin(PLAYER_BAUD, Serial, STARTUP_VOLUME)) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  audio_ready = true;
  const uint16_t total_files = audio.getTotalFile();
  Serial.print("Total files reported by DFPlayer: ");
  Serial.println(total_files);

  if (total_files > 0 && audio.playFileNum(1, &Serial)) {
    leds.startFillSweep(ANIMATION_STEP_DELAY_MS);
  }
}

void loop() {
  leds.update();

  if (millis() - last_sweep_ms >= SWEEP_PERIOD_MS) {
    last_sweep_ms = millis();
    leds.startFillSweep(ANIMATION_STEP_DELAY_MS);
  }

  if (!audio_ready) {
    leds.setStatus(false);
    return;
  }

  const bool is_playing = audio.isPlaying();
  leds.setStatus(is_playing);

  if (millis() - last_status_ms >= STATUS_PERIOD_MS) {
    last_status_ms = millis();
    Serial.print("Playing: ");
    Serial.println(is_playing ? "yes" : "no");
  }
}
