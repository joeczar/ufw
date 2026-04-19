#include "UfwPhrasePlayer.h"

UfwPhrasePlayer::UfwPhrasePlayer()
    : leds_(nullptr),
      phrase_(nullptr),
      running_(false),
      currentStepIndex_(0),
      activePin_(-1),
      stepStartedMs_(0) {}

void UfwPhrasePlayer::begin(UfwLed& leds) {
  leds_ = &leds;
}

void UfwPhrasePlayer::setPhrase(const UfwPhrase* phrase) {
  phrase_ = phrase;
}

void UfwPhrasePlayer::start() {
  stop();
  if (leds_ == nullptr || phrase_ == nullptr || phrase_->stepCount == 0) {
    return;
  }
  running_ = true;
  advanceToStep(0);
}

void UfwPhrasePlayer::stop() {
  if (leds_ != nullptr && activePin_ >= 0) {
    leds_->setPin(static_cast<uint8_t>(activePin_), false);
  }
  running_ = false;
  currentStepIndex_ = 0;
  activePin_ = -1;
  stepStartedMs_ = 0;
}

void UfwPhrasePlayer::update() {
  if (!running_ || leds_ == nullptr || phrase_ == nullptr) {
    return;
  }

  const UfwPhraseStep& step = phrase_->steps[currentStepIndex_];
  if (millis() - stepStartedMs_ < step.durationMs) {
    return;
  }

  if (activePin_ >= 0) {
    leds_->setPin(static_cast<uint8_t>(activePin_), false);
  }

  const size_t nextIndex = currentStepIndex_ + 1;
  if (nextIndex >= phrase_->stepCount) {
    stop();
    return;
  }

  advanceToStep(nextIndex);
}

bool UfwPhrasePlayer::isRunning() const {
  return running_;
}

void UfwPhrasePlayer::advanceToStep(size_t stepIndex) {
  currentStepIndex_ = stepIndex;
  activePin_ = phrase_->steps[stepIndex].ledPin;
  leds_->setPin(static_cast<uint8_t>(activePin_), true);
  stepStartedMs_ = millis();
}
