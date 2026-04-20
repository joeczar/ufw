# Design Notes: Phrases and Audio

Working notes — not finalized design. Captures what the current system already
expresses so we don't invent a new concept when the vocabulary already covers it.

## The immediate problem

`PhraseOnce` today goes through `UfwRuntime(Serial1, PHRASE_LED_PINS)` and
early-returns from `setup()` if `runtime.begin()` fails (which happens if the
DFPlayer isn't present or doesn't init). Result: an LED-only "phrase" example
silently runs nothing whenever the audio subsystem fails. That's bad for users
who legitimately want LED-only phrases.

Easy fix, no library change: rewrite `PhraseOnce` to instantiate
`UfwLed + UfwPhrasePlayer` directly, skipping `UfwRuntime`. The API already
supports this — `UfwPhrasePlayer::begin(UfwLed&)` doesn't require a runtime or
audio reference.

## The deeper question

> How should LED steps be tied to audio files?

Before adding anything, check what the current vocabulary already says.

### What `docs/technical-design.md` lists

From the Core Concepts section:

- **`Sound`** = `{name, trackNumber}` — named audio asset, one DFPlayer file.
- **`Phrase`** = `{name, steps[]}` where `step = {pin, durationMs}` — "a named
  LED pattern ... sequence of LED steps and durations." **Explicitly LED-only
  in the current type definition.**
- **`Cue`** = `{name, trackNumber, pin}` — "named audio-and-light pair. A cue
  links one sound to one LED."
- **`TrackPiece`** = long audio track + one-shot LED phrase at track start.
  Audio drives duration; phrase is a fixed intro animation.
- **`CuePiece`** = list of cues, audio-paced. Each cue's duration = "until its
  track ends." Shuffle keeps pairs intact.

### What the current vocabulary can express

| Artistic style | Current vocabulary | Who paces timing |
|---|---|---|
| "I remember this one time" — continuous poem + syllabic LED intro | `TrackPiece` + `Phrase` as onTrackStartPhrase | Audio paces, phrase is fixed-duration intro |
| "i / fuh / kuh / ing / hate / men" — atomic words, one recording each | `CuePiece` | Audio paces each cue |
| LED-only phrase (no audio at all) | `UfwLed + UfwPhrasePlayer` directly | Author (step durations) |

### What the vocabulary *does not* express today

A pattern where each step has:
- An LED
- A **fixed, author-controlled duration** (not "until audio ends")
- An **associated sound fired when the step starts**

Examples of this intent:
- A percussive sequence: "beep! blink! beep! blink!" with timing the author sets.
- An LED phrase for a poem line where each syllable also triggers a short
  per-syllable sound effect distinct from the poem recording.
- A tutorial/demo piece where LEDs+sound fire on a rhythm independent of the
  audio clip length.

There are **two ways** to fit this into the existing vocabulary. They are
structurally similar but philosophically different:

#### Path A — Extend `Phrase`

Make the step carry an optional audio track:

```cpp
struct UfwPhraseStep {
  uint8_t pin;
  uint16_t durationMs;
  uint16_t trackNumber;   // 0 = silent step
};
```

And upgrade `UfwPhrasePlayer::begin(UfwLed&, UfwAudio* = nullptr)` so that a
non-zero `trackNumber` fires audio when the step starts. Duration still
controls advance.

- Pro: one type, one player, optional audio. Existing silent phrases become
  `trackNumber = 0` (or the field is omitted at construction).
- Pro: keeps `UfwCuePiece` as the pure audio-paced model. No overlap.
- Con: phrase was defined as "LED pattern." Adding audio to the step
  contradicts the doc's current definition of the type. The doc would need
  updating.

#### Path B — Extend `Cue`

Make a cue carry an optional duration:

```cpp
struct UfwCue {
  const char* name;
  uint16_t trackNumber;
  uint8_t pin;
  uint16_t durationMs;   // 0 = advance when track ends (current behavior)
};
```

A cue piece with `durationMs > 0` becomes author-paced; a cue piece with
`durationMs = 0` stays audio-paced as today.

- Pro: `UfwCue` is already "sound + light" — adding duration is a natural
  author-control extension, not a type-concept collision.
- Pro: one piece type (`UfwCuePiece`) handles both pacing models. No new type.
- Con: `UfwCuePiece` semantics become conditional on per-cue data. Slightly
  more to document for authors.

#### Path C — Do nothing yet

Keep LED-only phrases working via `UfwLed + UfwPhrasePlayer` (the no-runtime
path). Defer the "voiced phrase" question until a real piece needs it. This
respects design rule #2 (YAGNI — "do not add a feature directly to one example
if it should be reusable elsewhere" cuts both ways).

## Things we might already have that I'd forgotten

Review before picking any path:

- **`UfwTrackPiece` accepts `const UfwPhrase* onTrackStartPhrase`** — a phrase
  runs once when a track starts. So "phrase tied to audio" *already exists*
  for the case where the audio is one long track and the phrase is the intro.
  This covers a lot of real intents. **Check whether the user's "tie each LED
  to an audio file" question is already solved by making one cue per LED in
  a `UfwCuePiece`, which is exactly what `IFuckingHateMen` does.**

- **`UfwCuePiece` already handles the per-LED audio case** — it walks a cue
  list, plays each cue's track, lights each cue's pin, waits for audio to end.
  If the user wants "each LED has its own audio file," the canonical answer is
  probably "write a cue piece," not "extend phrase."

- **The real novelty of the voiced phrase is the author-paced timing** —
  fixed step durations rather than waiting for audio. If you don't need
  author-paced timing, a cue piece is the right tool today. That means Path C
  might be entirely correct for now.

## Open questions for discussion

1. Does any concrete piece the user is planning *need* author-paced timing
   with per-step audio? Or is the intent covered by `UfwCuePiece`
   (audio-paced, per-word clips) + `UfwTrackPiece` (long track + fixed
   phrase intro)?

2. If the answer to (1) is "yes, there's a real need," which of Path A vs
   Path B keeps the vocabulary honest? The `technical-design.md` docs
   currently define phrase as LED-only and cue as audio+light — Path B
   aligns more with that wording. Path A would require revising the phrase
   definition.

3. Should we fix `PhraseOnce` now (so LED-only phrases work without
   DFPlayer) regardless of how the bigger voiced-phrase question lands?
   This is the cheap, correct change and blocks nothing.

## Related design rules (from technical-design.md)

- Rule 4: "Extend the design system, not the one-off sketch." New
  capabilities should slot into existing vocabulary.
- Rule 5: "Prefer composable behaviors over hidden special cases." Check
  the existing vocabulary covers an intent before adding a new concept.
- "LED matrix word display" is listed under "Rules For Future Features" as
  `output` or `phrase` extension — so extending phrase is explicitly allowed
  when it's the right home.
