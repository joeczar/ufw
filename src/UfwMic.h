#ifndef UFW_MIC_H
#define UFW_MIC_H

#include <Arduino.h>

struct UfwMicReading {
  int raw;
  int min;
  int max;
  int center;
  int peakToPeak;
  uint32_t sampleCount;
  uint16_t windowMs;
  uint32_t completedAtMs;
};

class UfwMic {
 public:
  explicit UfwMic(uint8_t analogPin, uint16_t windowMs = 100);

  void begin();
  void begin(uint8_t adcResolutionBits);
  void setWindowMs(uint16_t windowMs);
  uint16_t windowMs() const;
  uint8_t pin() const;
  void reset();
  bool update();
  bool hasReading() const;
  const UfwMicReading& reading() const;
  int raw() const;
  int min() const;
  int max() const;
  int center() const;
  int peakToPeak() const;
  uint32_t sampleCount() const;
  bool peakToPeakAtLeast(int threshold) const;

 private:
  void applyResolution(uint8_t adcResolutionBits);
  void resetAccumulator(uint32_t startedMs);
  void finalizeWindow(uint32_t nowMs);

  uint8_t analogPin_;
  uint16_t windowMs_;
  bool hasReading_;
  UfwMicReading latest_;
  uint32_t windowStartedMs_;
  uint32_t sampleSum_;
  uint32_t sampleCount_;
  int windowMin_;
  int windowMax_;
  int lastRaw_;
};

#endif
