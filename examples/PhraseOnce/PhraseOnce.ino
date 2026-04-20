/*
  PhraseOnce

  Smallest possible phrase example.
  No audio. A `UfwPhrase` runs once at boot: each step lights one LED for its
  duration, then moves to the next LED, then stops.

  This is the LED timing atom that `RememberThisOneTime` uses. You can copy the
  PHRASE_STEPS block below straight into a track piece once you are happy with
  the pacing.

  Edit:
  - PHRASE_LED_PINS   (which LEDs the phrase is allowed to light)
  - PHRASE_STEPS      (the sequence of {pin, how long the LED stays on in ms})

  You usually do not need to edit anything below the phrase list.
*/

#include <Ufw.h>

// --- Hardware settings: change these only if your board or wiring is different. ---
// These are the LED pins the phrase is allowed to light.
const uint8_t PHRASE_LED_PINS[] = {2, 4, 5, 6};

// --- Edit here: the phrase steps. ---
// Each step is:
// { LED pin, how long that LED stays on in milliseconds }
UfwPhraseStep PHRASE_STEPS[] = {
  {2, 200},
  {4, 200},
  {5, 200},
  {6, 200},
};

// Package the steps as a named phrase so the player can run it.
UfwPhrase PHRASE = {
  "demo phrase",
  PHRASE_STEPS,
  ufwCountOf(PHRASE_STEPS),
};

// --- Library objects: you usually do not need to edit below this line. ---
UfwRuntime runtime(Serial1, PHRASE_LED_PINS);
UfwPhrasePlayer phrasePlayer;

void setup() {
  if (!runtime.begin("Phrase Once")) {
    Serial.println("DFPlayer failed to initialize.");
    return;
  }

  phrasePlayer.begin(runtime.leds());
  phrasePlayer.setPhrase(&PHRASE);
  phrasePlayer.start();
}

void loop() {
  phrasePlayer.update();
}
