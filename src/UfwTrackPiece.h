#ifndef UFW_TRACK_PIECE_H
#define UFW_TRACK_PIECE_H

#include <Arduino.h>

#include "UfwAudio.h"
#include "UfwLed.h"
#include "UfwPhrasePlayer.h"
#include "UfwTypes.h"

class UfwTrackPiece {
 public:
  static constexpr size_t MAX_TRACKS = 64;

  UfwTrackPiece(const char* name, const UfwSound* sounds, size_t soundCount,
                const UfwPhrase* onTrackStartPhrase = nullptr);
  template <size_t N>
  UfwTrackPiece(const char* name, const UfwSound (&sounds)[N],
                const UfwPhrase* onTrackStartPhrase = nullptr)
      : UfwTrackPiece(name, sounds, N, onTrackStartPhrase) {}

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
  bool startSoundAtOrderIndex(size_t index);
  bool startNextSound();
  void finishPiece();

  const char* name_;
  const UfwSound* sounds_;
  size_t soundCount_;
  const UfwPhrase* onTrackStartPhrase_;
  UfwAudio* audio_;
  UfwLed* leds_;
  UfwPhrasePlayer phrasePlayer_;
  UfwOrderMode orderMode_;
  UfwFinishMode finishMode_;
  size_t playOrder_[MAX_TRACKS];
  size_t orderIndex_;
  bool started_;
  bool finished_;
  bool lastPlayingState_;
};

#endif
