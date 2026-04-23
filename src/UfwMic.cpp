#include "UfwMic.h"

UfwMic::UfwMic(uint8_t analogPin, uint16_t windowMs)
    : analogPin_(analogPin),
      windowMs_(windowMs),
      hasReading_(false),
      latest_{0, 0, 0, 0, 0, 0, windowMs, 0},
      windowStartedMs_(0),
      sampleSum_(0),
      sampleCount_(0),
      windowMin_(0),
      windowMax_(0),
      lastRaw_(0) {}

void UfwMic::begin() {
  pinMode(analogPin_, INPUT);
  reset();
}

void UfwMic::begin(uint8_t adcResolutionBits) {
  begin();
  applyResolution(adcResolutionBits);
}

void UfwMic::setWindowMs(uint16_t windowMs) {
  windowMs_ = windowMs;
  latest_.windowMs = windowMs_;
  reset();
}

uint16_t UfwMic::windowMs() const {
  return windowMs_;
}

uint8_t UfwMic::pin() const {
  return analogPin_;
}

void UfwMic::reset() {
  hasReading_ = false;
  latest_ = {0, 0, 0, 0, 0, 0, windowMs_, 0};
  resetAccumulator(millis());
}

bool UfwMic::update() {
  lastRaw_ = analogRead(analogPin_);

  if (sampleCount_ == 0) {
    windowMin_ = lastRaw_;
    windowMax_ = lastRaw_;
  } else {
    if (lastRaw_ < windowMin_) {
      windowMin_ = lastRaw_;
    }
    if (lastRaw_ > windowMax_) {
      windowMax_ = lastRaw_;
    }
  }

  sampleSum_ += static_cast<uint32_t>(lastRaw_);
  ++sampleCount_;

  const uint32_t nowMs = millis();
  if (windowMs_ > 0 && nowMs - windowStartedMs_ < windowMs_) {
    return false;
  }

  finalizeWindow(nowMs);
  resetAccumulator(nowMs);
  return true;
}

bool UfwMic::hasReading() const {
  return hasReading_;
}

const UfwMicReading& UfwMic::reading() const {
  return latest_;
}

int UfwMic::raw() const {
  return latest_.raw;
}

int UfwMic::min() const {
  return latest_.min;
}

int UfwMic::max() const {
  return latest_.max;
}

int UfwMic::center() const {
  return latest_.center;
}

int UfwMic::peakToPeak() const {
  return latest_.peakToPeak;
}

uint32_t UfwMic::sampleCount() const {
  return latest_.sampleCount;
}

bool UfwMic::peakToPeakAtLeast(int threshold) const {
  return hasReading_ && latest_.peakToPeak >= threshold;
}

void UfwMic::applyResolution(uint8_t adcResolutionBits) {
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD) || \
    defined(ARDUINO_ARCH_SAM)
  analogReadResolution(adcResolutionBits);
#else
  (void)adcResolutionBits;
#endif
}

void UfwMic::resetAccumulator(uint32_t startedMs) {
  windowStartedMs_ = startedMs;
  sampleSum_ = 0;
  sampleCount_ = 0;
  windowMin_ = 0;
  windowMax_ = 0;
}

void UfwMic::finalizeWindow(uint32_t nowMs) {
  if (sampleCount_ == 0) {
    return;
  }

  latest_.raw = lastRaw_;
  latest_.min = windowMin_;
  latest_.max = windowMax_;
  latest_.center = static_cast<int>(sampleSum_ / sampleCount_);
  latest_.peakToPeak = windowMax_ - windowMin_;
  latest_.sampleCount = sampleCount_;
  latest_.windowMs = windowMs_;
  latest_.completedAtMs = nowMs;
  hasReading_ = true;
}
