#ifndef UFW_TYPES_H
#define UFW_TYPES_H

#include <Arduino.h>

enum UfwOrderMode {
  UFW_IN_ORDER,
  UFW_SHUFFLE,
};

enum UfwFinishMode {
  UFW_STOP_WHEN_DONE,
  UFW_REPEAT_FOREVER,
};

struct UfwSound {
  const char* name;
  uint16_t trackNumber;
};

struct UfwPhraseStep {
  uint8_t ledPin;
  uint16_t durationMs;
};

struct UfwPhrase {
  const char* name;
  const UfwPhraseStep* steps;
  size_t stepCount;
};

struct UfwCue {
  const char* name;
  uint16_t trackNumber;
  uint8_t ledPin;
};

template <typename T, size_t N>
constexpr size_t ufwCountOf(const T (&)[N]) {
  return N;
}

#endif
