#ifndef UFW_CUE_PIECE_H
#define UFW_CUE_PIECE_H

#include <Arduino.h>

#include "UfwAudio.h"
#include "UfwLed.h"
#include "UfwTypes.h"

class UfwCuePiece {
 public:
  static constexpr size_t MAX_CUES = 64;

  UfwCuePiece(const char* name, const UfwCue* cues, size_t cueCount);
  template <size_t N>
  UfwCuePiece(const char* name, const UfwCue (&cues)[N])
      : UfwCuePiece(name, cues, N) {}

  void begin(UfwAudio& audio, UfwLed& leds);
  void shuffle();
  void inOrder();
  void stopWhenDone();
  void repeatForever();
  bool start();
  void update();
  bool isFinished() const;

 private:
  bool isReady() const;
  void buildPlayOrder();
  bool startCueAtOrderIndex(size_t index);
  bool startNextCue();
  void finishPiece();
  void setActiveCueLed(int pin);

  const char* name_;
  const UfwCue* cues_;
  size_t cueCount_;
  UfwAudio* audio_;
  UfwLed* leds_;
  UfwOrderMode orderMode_;
  UfwFinishMode finishMode_;
  size_t playOrder_[MAX_CUES];
  size_t orderIndex_;
  int activeCueLedPin_;
  bool started_;
  bool finished_;
  bool lastPlayingState_;
};

#endif
