/*
  PhraseWithSound

  One step above PhraseOnce: a phrase runs once *while* one audio track plays.
  This is the exact shape `UfwTrackPiece` wraps with shuffle, repeat, and
  multi-track support. Use this example when you want to see the moving parts
  before moving up to `RememberThisOneTime`.

  Edit:
  - TRACK_NUMBER      (which DFPlayer track plays)
  - PHRASE_LED_PINS   (which LEDs the phrase is allowed to light)
  - PHRASE_STEPS      (the sequence of {pin, duration in ms})

  You usually do not need to edit anything below the phrase list.
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board or wiring is different. ---
const uint8_t PHRASE_LED_PINS[] = {2, 4, 5, 6};

// --- Edit here: the sound and the phrase that runs alongside it. ---
const uint16_t TRACK_NUMBER = 1;

UfwPhraseStep PHRASE_STEPS[] = {
  {2, 200},
  {4, 200},
  {5, 200},
  {6, 200},
};

UfwPhrase PHRASE = {
  "demo phrase",
  PHRASE_STEPS,
  ufwCountOf(PHRASE_STEPS),
};

// --- Library objects: you usually do not need to edit below this line. ---
UfwRuntime runtime(Serial1, PHRASE_LED_PINS);
UfwPhrasePlayer phrasePlayer;

void setup() {
  if (!runtime.begin("Phrase With Sound")) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  phrasePlayer.begin(runtime.leds());
  phrasePlayer.setPhrase(&PHRASE);

  if (runtime.audio().playFileNum(TRACK_NUMBER, &Serial)) {
    phrasePlayer.start();
  }
}

void loop() {
  phrasePlayer.update();
}
