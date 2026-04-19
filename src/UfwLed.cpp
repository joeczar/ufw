#include "UfwLed.h"

UfwLed::UfwLed(const uint8_t* animationPins, size_t animationPinCount,
               int statusPin, int builtinPin)
    : animationPins_(animationPins),
      animationPinCount_(animationPinCount),
      statusPin_(statusPin),
      builtinPin_(builtinPin),
      animationPhase_(kIdle),
      animationIndex_(0),
      stepDelayMs_(0),
      brightnessStep_(0),
      brightness_(0),
      lastStepMs_(0) {}

void UfwLed::begin() {
  for (size_t i = 0; i < animationPinCount_; ++i) {
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

void UfwLed::startFillSweep(uint16_t stepDelayMs) {
  writeAllAnimationPins(false);
  animationPhase_ = kFillOn;
  animationIndex_ = 0;
  stepDelayMs_ = stepDelayMs;
  brightnessStep_ = 0;
  brightness_ = 0;
  lastStepMs_ = millis();
}

void UfwLed::startFadeSweep(uint16_t frameDelayMs, uint8_t brightnessStep) {
  writeAllAnimationPins(false);
  animationPhase_ = kFadeUp;
  animationIndex_ = 0;
  stepDelayMs_ = frameDelayMs;
  brightnessStep_ = brightnessStep == 0 ? 1 : brightnessStep;
  brightness_ = 0;
  lastStepMs_ = millis();
}

void UfwLed::stopAnimation() {
  animationPhase_ = kIdle;
  animationIndex_ = 0;
  brightnessStep_ = 0;
  brightness_ = 0;
  writeAllAnimationPins(false);
}

void UfwLed::update() {
  if (animationPhase_ == kIdle || animationPinCount_ == 0) {
    return;
  }

  if (millis() - lastStepMs_ < stepDelayMs_) {
    return;
  }
  lastStepMs_ = millis();

  if (animationPhase_ == kFillOn) {
    digitalWrite(animationPins_[animationIndex_], HIGH);
    ++animationIndex_;
    if (animationIndex_ >= animationPinCount_) {
      animationPhase_ = kFillOff;
      animationIndex_ = 0;
    }
    return;
  }

  if (animationPhase_ == kFadeUp) {
    brightness_ += brightnessStep_;
    if (brightness_ > 255) {
      brightness_ = 255;
    }
    analogWrite(animationPins_[animationIndex_], brightness_);
    if (brightness_ >= 255) {
      animationPhase_ = kFadeDown;
    }
    return;
  }

  if (animationPhase_ == kFadeDown) {
    brightness_ -= brightnessStep_;
    if (brightness_ < 0) {
      brightness_ = 0;
    }
    analogWrite(animationPins_[animationIndex_], brightness_);
    if (brightness_ <= 0) {
      ++animationIndex_;
      if (animationIndex_ >= animationPinCount_) {
        animationPhase_ = kIdle;
        animationIndex_ = 0;
        return;
      }
      animationPhase_ = kFadeUp;
    }
    return;
  }

  digitalWrite(animationPins_[animationIndex_], LOW);
  ++animationIndex_;
  if (animationIndex_ >= animationPinCount_) {
    animationPhase_ = kIdle;
    animationIndex_ = 0;
  }
}

bool UfwLed::isAnimating() const {
  return animationPhase_ != kIdle;
}

bool UfwLed::hasAnimationPin(uint8_t pin) const {
  for (size_t i = 0; i < animationPinCount_; ++i) {
    if (animationPins_[i] == pin) {
      return true;
    }
  }
  return false;
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
  for (size_t i = 0; i < animationPinCount_; ++i) {
    digitalWrite(animationPins_[i], on ? HIGH : LOW);
  }
}
