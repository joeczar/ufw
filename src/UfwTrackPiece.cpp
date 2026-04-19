#include "UfwTrackPiece.h"

namespace {
constexpr uint16_t kPauseDelayMs = 150;
}

UfwTrackPiece::UfwTrackPiece(const char* name, const UfwSound* sounds,
                             size_t soundCount,
                             const UfwPhrase* onTrackStartPhrase)
    : name_(name),
      sounds_(sounds),
      soundCount_(soundCount > MAX_TRACKS ? MAX_TRACKS : soundCount),
      onTrackStartPhrase_(onTrackStartPhrase),
      audio_(nullptr),
      leds_(nullptr),
      orderMode_(UFW_IN_ORDER),
      finishMode_(UFW_STOP_WHEN_DONE),
      orderIndex_(0),
      started_(false),
      finished_(false),
      lastPlayingState_(false) {}

void UfwTrackPiece::begin(UfwAudio& audio, UfwLed& leds) {
  audio_ = &audio;
  leds_ = &leds;
  phrasePlayer_.begin(leds);
  phrasePlayer_.setPhrase(onTrackStartPhrase_);
}

void UfwTrackPiece::shuffle() {
  orderMode_ = UFW_SHUFFLE;
}

void UfwTrackPiece::inOrder() {
  orderMode_ = UFW_IN_ORDER;
}

void UfwTrackPiece::stopWhenDone() {
  finishMode_ = UFW_STOP_WHEN_DONE;
}

void UfwTrackPiece::repeatForever() {
  finishMode_ = UFW_REPEAT_FOREVER;
}

bool UfwTrackPiece::isReady() const {
  return audio_ != nullptr && leds_ != nullptr;
}

bool UfwTrackPiece::start() {
  if (!isReady() || soundCount_ == 0) {
    finished_ = true;
    return false;
  }

  buildPlayOrder();
  orderIndex_ = 0;
  started_ = false;
  finished_ = false;
  lastPlayingState_ = false;
  return startNextSound();
}

void UfwTrackPiece::update() {
  phrasePlayer_.update();

  if (!isReady() || finished_) {
    return;
  }

  const bool isPlaying = audio_->isPlaying();
  if (started_ && lastPlayingState_ && !isPlaying) {
    audio_->pause(nullptr);
    delay(kPauseDelayMs);
    startNextSound();
    return;
  }

  lastPlayingState_ = isPlaying;
  leds_->setStatus(isPlaying);
}

bool UfwTrackPiece::isFinished() const {
  return finished_;
}

void UfwTrackPiece::buildPlayOrder() {
  for (size_t i = 0; i < soundCount_; ++i) {
    playOrder_[i] = i;
  }

  if (orderMode_ != UFW_SHUFFLE) {
    return;
  }

  for (int i = static_cast<int>(soundCount_) - 1; i > 0; --i) {
    const int j = random(i + 1);
    const size_t temp = playOrder_[i];
    playOrder_[i] = playOrder_[j];
    playOrder_[j] = temp;
  }
}

bool UfwTrackPiece::startSoundAtOrderIndex(size_t index) {
  const UfwSound& sound = sounds_[playOrder_[index]];

  if (!audio_->playFileNum(sound.trackNumber, &Serial)) {
    leds_->setStatus(false);
    return false;
  }

  if (name_ != nullptr) {
    Serial.print(name_);
    Serial.print(": ");
  }
  Serial.print("Starting ");
  Serial.print(sound.name);
  Serial.print(" on track ");
  Serial.println(sound.trackNumber);

  leds_->setStatus(true);
  phrasePlayer_.start();
  started_ = true;
  lastPlayingState_ = true;
  return true;
}

bool UfwTrackPiece::startNextSound() {
  if (orderIndex_ >= soundCount_) {
    if (finishMode_ == UFW_REPEAT_FOREVER) {
      buildPlayOrder();
      orderIndex_ = 0;
    } else {
      finishPiece();
      return false;
    }
  }

  const bool started = startSoundAtOrderIndex(orderIndex_);
  if (started) {
    ++orderIndex_;
  }
  return started;
}

void UfwTrackPiece::finishPiece() {
  finished_ = true;
  phrasePlayer_.stop();
  leds_->setStatus(false);
  if (name_ != nullptr) {
    Serial.print(name_);
    Serial.println(": finished");
  }
}
