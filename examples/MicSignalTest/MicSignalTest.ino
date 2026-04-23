/*
  MicSignalTest

  This is a bench test for an analog microphone breakout such as a MAX4466.
  It reads the mic on `MIC_PIN` and prints rolling Serial measurements so you
  can inspect the quiet bias level and the sound-reactive swing before building
  a reusable trigger into the library.

  What it shows:
  - the live raw ADC reading
  - the rolling minimum and maximum over one report window
  - the estimated center bias over that same window
  - the peak-to-peak swing, which is the easiest number to tune against
  - a simple text meter so loudness changes are easy to spot in Serial Monitor

  Wiring for the current bench setup:
  - mic `VCC` -> Pico W `3V3`
  - mic `GND` -> Pico W `GND`
  - mic `OUT` -> Pico W `GP26`

  Edit:
  - `MIC_PIN` if your mic output is wired to a different analog pin
  - `REPORT_INTERVAL_MS` if you want faster or slower logging
*/

#include <Ufw.h>

const int USB_BAUD = 115200;
const int MIC_PIN = 26;
const uint16_t REPORT_INTERVAL_MS = 100;
const size_t METER_WIDTH = 24;
const int ADC_MAX = 4095;

UfwMic mic(MIC_PIN, REPORT_INTERVAL_MS);

void printMeter(int peak_to_peak) {
  const long filled =
      map(constrain(peak_to_peak, 0, ADC_MAX), 0, ADC_MAX, 0, METER_WIDTH);

  Serial.print('[');
  for (size_t i = 0; i < METER_WIDTH; ++i) {
    Serial.print(static_cast<long>(i) < filled ? '#' : '.');
  }
  Serial.print(']');
}

void printWindowStats() {
  if (!mic.hasReading()) {
    return;
  }

  const int peak_to_peak = mic.peakToPeak();

  Serial.print("raw=");
  Serial.print(mic.raw());
  Serial.print(" center=");
  Serial.print(mic.center());
  Serial.print(" min=");
  Serial.print(mic.min());
  Serial.print(" max=");
  Serial.print(mic.max());
  Serial.print(" p2p=");
  Serial.print(peak_to_peak);
  Serial.print(" samples=");
  Serial.print(mic.sampleCount());
  Serial.print(' ');
  printMeter(peak_to_peak);
  Serial.println();
}

void setup() {
  Serial.begin(USB_BAUD);
  Serial.ignoreFlowControl(true);
  delay(1500);
  Serial.println();
  Serial.println("=== Mic Signal Test ===");
  Serial.println("Reading analog mic on GP26");
  Serial.println("Columns: raw, center, min, max, p2p, samples");

  mic.begin(12);
}

void loop() {
  if (!mic.update()) {
    return;
  }

  printWindowStats();
}
