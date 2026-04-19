#include "UfwCuePiece.h"

namespace {
constexpr uint16_t kPauseDelayMs = 150;
}

UfwCuePiece::UfwCuePiece(const char* name, const UfwCue* cues, size_t cueCount)
    : name_(name),
      cues_(cues),
      cueCount_(cueCount > MAX_CUES ? MAX_CUES : cueCount),
      audio_(nullptr),
      leds_(nullptr),
      orderMode_(UFW_IN_ORDER),
      finishMode_(UFW_REPEAT_FOREVER),
      orderIndex_(0),
      activeCueLedPin_(-1),
      started_(false),
      finished_(false),
      lastPlayingState_(false) {}

void UfwCuePiece::begin(UfwAudio& audio, UfwLed& leds) {
  audio_ = &audio;
  leds_ = &leds;
}

void UfwCuePiece::shuffle() {
  orderMode_ = UFW_SHUFFLE;
}

void UfwCuePiece::inOrder() {
  orderMode_ = UFW_IN_ORDER;
}

void UfwCuePiece::stopWhenDone() {
  finishMode_ = UFW_STOP_WHEN_DONE;
}

void UfwCuePiece::repeatForever() {
  finishMode_ = UFW_REPEAT_FOREVER;
}

bool UfwCuePiece::isReady() const {
  return audio_ != nullptr && leds_ != nullptr;
}

bool UfwCuePiece::start() {
  if (!isReady() || cueCount_ == 0) {
    finished_ = true;
    return false;
  }

  buildPlayOrder();
  orderIndex_ = 0;
  started_ = false;
  finished_ = false;
  lastPlayingState_ = false;
  setActiveCueLed(-1);
  return startNextCue();
}

void UfwCuePiece::update() {
  if (!isReady() || finished_) {
    return;
  }

  const bool isPlaying = audio_->isPlaying();
  if (started_ && lastPlayingState_ && !isPlaying) {
    audio_->pause(nullptr);
    delay(kPauseDelayMs);
    startNextCue();
    return;
  }

  lastPlayingState_ = isPlaying;
  leds_->setStatus(isPlaying);
}

bool UfwCuePiece::isFinished() const {
  return finished_;
}

void UfwCuePiece::buildPlayOrder() {
  for (size_t i = 0; i < cueCount_; ++i) {
    playOrder_[i] = i;
  }

  if (orderMode_ != UFW_SHUFFLE) {
    return;
  }

  for (int i = static_cast<int>(cueCount_) - 1; i > 0; --i) {
    const int j = random(i + 1);
    const size_t temp = playOrder_[i];
    playOrder_[i] = playOrder_[j];
    playOrder_[j] = temp;
  }
}

bool UfwCuePiece::startCueAtOrderIndex(size_t index) {
  const UfwCue& cue = cues_[playOrder_[index]];

  if (!audio_->playFileNum(cue.trackNumber, &Serial)) {
    leds_->setStatus(false);
    return false;
  }

  setActiveCueLed(cue.ledPin);
  leds_->setStatus(true);
  started_ = true;
  lastPlayingState_ = true;

  if (name_ != nullptr) {
    Serial.print(name_);
    Serial.print(": ");
  }
  Serial.print("Starting ");
  Serial.print(cue.name);
  Serial.print(" on track ");
  Serial.print(cue.trackNumber);
  Serial.print(" with LED ");
  Serial.println(cue.ledPin);
  return true;
}

bool UfwCuePiece::startNextCue() {
  if (orderIndex_ >= cueCount_) {
    if (finishMode_ == UFW_REPEAT_FOREVER) {
      buildPlayOrder();
      orderIndex_ = 0;
    } else {
      finishPiece();
      return false;
    }
  }

  const bool started = startCueAtOrderIndex(orderIndex_);
  if (started) {
    ++orderIndex_;
  }
  return started;
}

void UfwCuePiece::finishPiece() {
  finished_ = true;
  setActiveCueLed(-1);
  leds_->setStatus(false);
  if (name_ != nullptr) {
    Serial.print(name_);
    Serial.println(": finished");
  }
}

void UfwCuePiece::setActiveCueLed(int pin) {
  if (activeCueLedPin_ >= 0) {
    leds_->setPin(static_cast<uint8_t>(activeCueLedPin_), false);
  }
  activeCueLedPin_ = pin;
  if (activeCueLedPin_ >= 0) {
    leds_->setPin(static_cast<uint8_t>(activeCueLedPin_), true);
  }
}
