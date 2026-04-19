#ifndef UFW_PHRASE_PLAYER_H
#define UFW_PHRASE_PLAYER_H

#include <Arduino.h>

#include "UfwLed.h"
#include "UfwTypes.h"

class UfwPhrasePlayer {
 public:
  UfwPhrasePlayer();

  void begin(UfwLed& leds);
  void setPhrase(const UfwPhrase* phrase);
  void start();
  void stop();
  void update();
  bool isRunning() const;

 private:
  void advanceToStep(size_t stepIndex);

  UfwLed* leds_;
  const UfwPhrase* phrase_;
  bool running_;
  size_t currentStepIndex_;
  int activePin_;
  uint32_t stepStartedMs_;
};

#endif
