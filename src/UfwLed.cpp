#include "UfwLed.h"

UfwLed::UfwLed(const uint8_t* animationPins, size_t animationPinCount,
               int statusPin, int builtinPin)
    : animationPins_(animationPins),
      animationPinCount_(animationPinCount),
      statusPin_(statusPin),
      builtinPin_(builtinPin),
      animationMode_(kIdle),
      sweepStep_(0),
      maxLit_(0),
      stepDelayMs_(0),
      brightness_(255),
      brightnessProfile_{nullptr, 0},
      brightnessStep_(0),
      lastStepMs_(0),
      breathePeakBrightness_(255),
      breatheIncreasing_(true),
      breatheCyclesRemaining_(0) {
  resetBrightnessState();
}

void UfwLed::begin() {
  for (size_t i = 0; i < managedAnimationPinCount(); ++i) {
    pinMode(animationPins_[i], OUTPUT);
    digitalWrite(animationPins_[i], LOW);
  }

  if (builtinPin_ >= 0) {
    pinMode(builtinPin_, OUTPUT);
    digitalWrite(builtinPin_, LOW);
  }

  if (statusPin_ >= 0) {
    pinMode(statusPin_, OUTPUT);
    digitalWrite(statusPin_, LOW);
  }
}

void UfwLed::clear() {
  stopAnimation();
  writeAllAnimationPins(false);
  writeStatusPins(false);
}

void UfwLed::setPin(uint8_t pin, bool on) {
  if (!hasAnimationPin(pin)) {
    return;
  }
  digitalWrite(pin, on ? HIGH : LOW);
}

void UfwLed::setStatus(bool on) {
  writeStatusPins(on);
}

void UfwLed::pulseBootPattern(uint8_t pulses, uint16_t onMs, uint16_t offMs) {
  for (uint8_t i = 0; i < pulses; ++i) {
    writeStatusPins(true);
    delay(onMs);
    writeStatusPins(false);
    delay(offMs);
  }
}

void UfwLed::startFillSweep(uint16_t stepDelayMs, size_t maxLit,
                            uint8_t brightness) {
  startFillAnimation(stepDelayMs, maxLit, brightness, {nullptr, 0});
}

void UfwLed::startFillSweep(uint16_t stepDelayMs, size_t maxLit,
                            UfwBrightnessProfile brightnessProfile) {
  startFillAnimation(stepDelayMs, maxLit, 255, brightnessProfile);
}

void UfwLed::startFadeSweep(uint16_t frameDelayMs, size_t maxLit,
                            uint8_t brightness, uint8_t fadeStep) {
  startWindowAnimation(kFadeSweep, frameDelayMs, maxLit, brightness,
                       {nullptr, 0}, fadeStep);
}

void UfwLed::startFadeSweep(uint16_t frameDelayMs, size_t maxLit,
                            UfwBrightnessProfile brightnessProfile,
                            uint8_t fadeStep) {
  startWindowAnimation(kFadeSweep, frameDelayMs, maxLit, 255,
                       brightnessProfile, fadeStep);
}

void UfwLed::startComet(uint16_t frameDelayMs, size_t maxLit,
                        uint8_t brightness, uint8_t fadeStep) {
  startWindowAnimation(kComet, frameDelayMs, maxLit, brightness, {nullptr, 0},
                       fadeStep);
}

void UfwLed::startComet(uint16_t frameDelayMs, size_t maxLit,
                        UfwBrightnessProfile brightnessProfile,
                        uint8_t fadeStep) {
  startWindowAnimation(kComet, frameDelayMs, maxLit, 255, brightnessProfile,
                       fadeStep);
}

void UfwLed::startBreathe(uint16_t frameDelayMs, uint8_t brightness,
                          uint8_t fadeStep) {
  startBreatheCommon(frameDelayMs, -1, brightness, fadeStep);
}

void UfwLed::startBreatheOnce(uint16_t frameDelayMs, uint8_t brightness,
                              uint8_t fadeStep) {
  startBreatheCommon(frameDelayMs, 1, brightness, fadeStep);
}

void UfwLed::startBreatheCount(uint16_t frameDelayMs, uint16_t pulseCount,
                               uint8_t brightness, uint8_t fadeStep) {
  if (pulseCount == 0) {
    stopAnimation();
    return;
  }
  startBreatheCommon(frameDelayMs, pulseCount, brightness, fadeStep);
}

void UfwLed::stopAnimation() {
  animationMode_ = kIdle;
  sweepStep_ = 0;
  maxLit_ = 0;
  brightness_ = 255;
  brightnessProfile_ = {nullptr, 0};
  brightnessStep_ = 0;
  breathePeakBrightness_ = 255;
  breatheIncreasing_ = true;
  breatheCyclesRemaining_ = 0;
  resetBrightnessState();
  writeAllAnimationPins(false);
}

void UfwLed::update() {
  if (animationMode_ == kIdle || animationPinCount_ == 0) {
    return;
  }

  if (millis() - lastStepMs_ < stepDelayMs_) {
    return;
  }
  lastStepMs_ = millis();

  if (animationMode_ == kFillSweep) {
    if (sweepStep_ >= maxSweepStep()) {
      stopAnimation();
      return;
    }
    ++sweepStep_;
    renderAnimationFrame();
    return;
  }

  if (animationMode_ == kBreathe) {
    if (fadeFrameSettled()) {
      if (breatheIncreasing_) {
        breatheIncreasing_ = false;
        setAllTargets(0);
      } else {
        if (breatheCyclesRemaining_ > 0) {
          --breatheCyclesRemaining_;
        }
        if (breatheCyclesRemaining_ == 0) {
          stopAnimation();
          return;
        }
        breatheIncreasing_ = true;
        setAllTargets(breathePeakBrightness_);
      }
    }
    stepFadeFrame();
    renderFadeFrame();
    return;
  }

  if (fadeFrameSettled()) {
    ++sweepStep_;
    refreshFadeTargets();
  }
  stepFadeFrame();
  renderFadeFrame();

  if (sweepStep_ > maxSweepStep() && fadeFrameSettled()) {
    stopAnimation();
  }
}

bool UfwLed::isAnimating() const {
  return animationMode_ != kIdle;
}

bool UfwLed::hasAnimationPin(uint8_t pin) const {
  for (size_t i = 0; i < managedAnimationPinCount(); ++i) {
    if (animationPins_[i] == pin) {
      return true;
    }
  }
  return false;
}

size_t UfwLed::managedAnimationPinCount() const {
  return animationPinCount_ > kMaxTrackedPins ? kMaxTrackedPins
                                              : animationPinCount_;
}

size_t UfwLed::activeWindowSize() const {
  const size_t pinCount = managedAnimationPinCount();
  size_t window = maxLit_ == 0 ? pinCount : maxLit_;
  if (window > pinCount) {
    window = pinCount;
  }
  if (brightnessProfile_.levels != nullptr && brightnessProfile_.count > 0 &&
      window > brightnessProfile_.count) {
    window = brightnessProfile_.count;
  }
  return window;
}

size_t UfwLed::maxSweepStep() const {
  const size_t window = activeWindowSize();
  const size_t pinCount = managedAnimationPinCount();
  if (pinCount == 0 || window == 0) {
    return 0;
  }
  return pinCount + window - 2;
}

uint8_t UfwLed::windowBrightness(size_t windowPosition) const {
  if (brightnessProfile_.levels != nullptr && brightnessProfile_.count > 0) {
    if (windowPosition >= brightnessProfile_.count) {
      return 0;
    }
    return brightnessProfile_.levels[windowPosition];
  }
  return brightness_;
}

void UfwLed::resetBrightnessState() {
  for (size_t i = 0; i < kMaxTrackedPins; ++i) {
    currentBrightness_[i] = 0;
    targetBrightness_[i] = 0;
  }
}

void UfwLed::startFillAnimation(uint16_t stepDelayMs, size_t maxLit,
                                uint8_t brightness,
                                UfwBrightnessProfile brightnessProfile) {
  stopAnimation();
  animationMode_ = kFillSweep;
  sweepStep_ = 0;
  maxLit_ = maxLit;
  stepDelayMs_ = stepDelayMs;
  brightness_ = brightness;
  brightnessProfile_ = brightnessProfile;
  brightnessStep_ = 0;
  lastStepMs_ = millis();
  resetBrightnessState();
  renderAnimationFrame();
}

void UfwLed::setAllTargets(uint8_t brightness) {
  const size_t pinCount = managedAnimationPinCount();
  for (size_t i = 0; i < pinCount; ++i) {
    targetBrightness_[i] = brightness;
  }
  for (size_t i = pinCount; i < kMaxTrackedPins; ++i) {
    targetBrightness_[i] = 0;
  }
}

void UfwLed::startWindowAnimation(AnimationMode mode, uint16_t frameDelayMs,
                                  size_t maxLit, uint8_t brightness,
                                  UfwBrightnessProfile brightnessProfile,
                                  uint8_t fadeStep) {
  stopAnimation();
  animationMode_ = mode;
  sweepStep_ = 0;
  maxLit_ = maxLit;
  stepDelayMs_ = frameDelayMs;
  brightness_ = brightness;
  brightnessProfile_ = brightnessProfile;
  brightnessStep_ = fadeStep == 0 ? 1 : fadeStep;
  lastStepMs_ = millis();
  resetBrightnessState();
  refreshFadeTargets();
  renderFadeFrame();
}

void UfwLed::startBreatheCommon(uint16_t frameDelayMs, int pulseCount,
                                uint8_t brightness, uint8_t fadeStep) {
  stopAnimation();
  animationMode_ = kBreathe;
  stepDelayMs_ = frameDelayMs;
  brightness_ = brightness;
  brightnessProfile_ = {nullptr, 0};
  brightnessStep_ = fadeStep == 0 ? 1 : fadeStep;
  lastStepMs_ = millis();
  breathePeakBrightness_ = brightness;
  breatheIncreasing_ = true;
  breatheCyclesRemaining_ = pulseCount;
  resetBrightnessState();
  setAllTargets(breathePeakBrightness_);
  renderFadeFrame();
}

void UfwLed::refreshFadeTargets() {
  for (size_t i = 0; i < kMaxTrackedPins; ++i) {
    targetBrightness_[i] = 0;
  }

  const size_t window = activeWindowSize();
  const size_t pinCount = managedAnimationPinCount();
  if ((animationMode_ != kFadeSweep && animationMode_ != kComet) ||
      pinCount == 0 || window == 0 || sweepStep_ > maxSweepStep()) {
    return;
  }

  const size_t startIndex =
      sweepStep_ >= (window - 1) ? sweepStep_ - (window - 1) : 0;
  const size_t endIndex = sweepStep_ < pinCount ? sweepStep_ : pinCount - 1;

  for (size_t ledIndex = startIndex; ledIndex <= endIndex; ++ledIndex) {
    const size_t windowPosition = sweepStep_ - ledIndex;
    targetBrightness_[ledIndex] = windowBrightness(windowPosition);
  }
}

bool UfwLed::fadeFrameSettled() const {
  const size_t pinCount = managedAnimationPinCount();
  for (size_t i = 0; i < pinCount; ++i) {
    if (currentBrightness_[i] != targetBrightness_[i]) {
      return false;
    }
  }
  return true;
}

void UfwLed::stepFadeFrame() {
  const size_t pinCount = managedAnimationPinCount();
  for (size_t i = 0; i < pinCount; ++i) {
    const uint8_t current = currentBrightness_[i];
    const uint8_t target = targetBrightness_[i];

    if (current == target) {
      continue;
    }

    if (current < target) {
      const uint16_t next = static_cast<uint16_t>(current) + brightnessStep_;
      currentBrightness_[i] =
          next > target ? target : static_cast<uint8_t>(next);
    } else {
      const int next = static_cast<int>(current) - brightnessStep_;
      currentBrightness_[i] =
          next < target ? target : static_cast<uint8_t>(next);
    }
  }
}

void UfwLed::renderFadeFrame() {
  const size_t pinCount = managedAnimationPinCount();
  for (size_t i = 0; i < pinCount; ++i) {
    writeAnimationPin(animationPins_[i], currentBrightness_[i]);
  }
}

void UfwLed::renderAnimationFrame() {
  writeAllAnimationPins(false);

  const size_t window = activeWindowSize();
  const size_t pinCount = managedAnimationPinCount();
  if (animationMode_ == kIdle || pinCount == 0 || window == 0 ||
      sweepStep_ > maxSweepStep()) {
    return;
  }

  const size_t startIndex =
      sweepStep_ >= (window - 1) ? sweepStep_ - (window - 1) : 0;
  const size_t endIndex =
      sweepStep_ < pinCount ? sweepStep_ : pinCount - 1;

  for (size_t ledIndex = startIndex; ledIndex <= endIndex; ++ledIndex) {
    const size_t windowPosition = sweepStep_ - ledIndex;
    const uint8_t brightness = windowBrightness(windowPosition);
    if (brightness == 0) {
      continue;
    }
    writeAnimationPin(animationPins_[ledIndex], brightness);
  }
}

void UfwLed::writeAnimationPin(uint8_t pin, uint8_t brightness) {
  analogWrite(pin, brightness);
}

void UfwLed::writeStatusPins(bool on) {
  if (builtinPin_ >= 0) {
    digitalWrite(builtinPin_, on ? HIGH : LOW);
  }
  if (statusPin_ >= 0) {
    digitalWrite(statusPin_, on ? HIGH : LOW);
  }
}

void UfwLed::writeAllAnimationPins(bool on) {
  for (size_t i = 0; i < managedAnimationPinCount(); ++i) {
    if (on) {
      analogWrite(animationPins_[i], 255);
    } else {
      analogWrite(animationPins_[i], 0);
    }
  }
}
