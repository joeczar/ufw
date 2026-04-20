# Ufw Technical Design

`Ufw` is a design system for building interactive audio-and-light pieces on Arduino hardware. The goal is not just to provide hardware wrappers, but to make new pieces feel like assembling the same small set of parts in different combinations.

## Design Rules

1. Keep the system layered.
   - `hardware adapters`: direct board and device wrappers such as `UfwAudio` and `UfwLed`
   - `assets`: named reusable content such as sounds, phrases, and cues
   - `patterns`: ordering and repetition rules such as `shuffle`, `repeat`, `stop`, and `once`
   - `pieces`: a specific artwork built from assets plus patterns
   - `triggers/extensions`: future inputs and outputs such as NFC, capacitive touch, or LED matrix behaviors

2. Keep the friend-facing surface data-first.
   - A collaborator making a new piece should mostly edit named lists and settings.
   - They should not need to manage DFPlayer state, playback transitions, pin bookkeeping, or shuffle logic.

3. Separate readability from runtime truth.
   - Examples should use named assets like `poem1`, `remember_phrase`, or `meh`.
   - Runtime code can still resolve those names to DFPlayer track numbers and GPIO pins.

4. Extend the design system, not the one-off sketch.
   - New capabilities should slot into the same vocabulary rather than adding piece-specific logic to a single example.
   - Example: NFC should become a trigger, not a custom branch inside one poem sketch.

5. Prefer composable behaviors over hidden special cases.
   - `shuffle`, `in order`, `repeat`, `stop`, and `on start` should be reusable behaviors.
   - Future features should build on those same behaviors whenever possible.

## Core Concepts

### Sound
A named audio asset that maps to one DFPlayer track number.

Current representation:

```cpp
UfwSound poem1 = {"poem1", 1};
```

### Phrase
A named LED pattern. A phrase is a sequence of LED steps and durations.

Current representation:

```cpp
UfwPhraseStep rememberPhrase[] = {
  {2, 120},
  {4, 120},
  {5, 120},
};
```

### Cue
A named audio-and-light pair. A cue links one sound to one LED.

Current representation:

```cpp
UfwCue cue = {"meh", 7, 8};
```

### TrackPiece
A piece built from longer tracks. It usually:
- plays one track at a time
- applies a pattern such as `shuffle` or `in order`
- may trigger an LED phrase once when a track starts
- may stop or repeat when the list finishes

`RememberThisOneTime` is the reference pattern for this kind of piece.

### CuePiece
A piece built from short sound-and-light units. It usually:
- walks through a cue list
- plays the cue's sound
- lights the cue's LED
- uses a pattern such as `shuffle`
- often repeats forever

`IFuckingHateMen` is the reference pattern for this kind of piece.

### LED Animation Helper
`UfwLed` is the low-level light engine used underneath phrases and direct LED
tests.

Current built-in animation vocabulary:

- `fill sweep`
- `fade sweep`
- `breathe`

Rules for the LED helper:

- all built-in animations must be non-blocking
- sweep-style animations must support `maxLit`
- sweep-style animations may use either scalar brightness or a brightness
  profile array
- brightness profile arrays are ordered from newest `head` LED to oldest
  `tail` LED
- direct LED examples should use `LedHardwareTest` as the reference teaching
  surface

## Current Implementation Shape

The library now exposes:

- `UfwAudio`
- `UfwLed`
- `UfwFileTracker`
- `UfwPhrasePlayer`
- `UfwTrackPiece`
- `UfwCuePiece`
- lightweight design-system types in `UfwTypes.h`

The canonical examples should use the piece classes instead of rebuilding playback state by hand.

The current `UfwLed` implementation uses a per-LED `currentBrightness ->
targetBrightness` model. That means new animations should usually be built by:

1. deciding target brightness for each LED
2. stepping current brightness toward those targets
3. rendering in `update()`

New LED patterns should extend that shared model rather than adding blocking
animation code to examples.

## Rules For Future Features

When adding a new capability, decide which layer it belongs to.

Examples:
- NFC tag start/selection: `trigger`
- capacitive touch advance/restart: `trigger`
- LED matrix word display: `output` or `phrase` extension
- alternate playback orders: `pattern`
- named groups of sounds: `asset`

Do not add a feature directly to one example if it should be reusable elsewhere.

## Public API Guidance

The public authoring surface should aim for:
- readable names
- explicit asset lists
- small setup/loop sections
- update-driven behavior
- piece classes that own control flow

The public authoring surface should avoid exposing:
- DFPlayer retry and timeout details
- raw loop-state variables unless they are part of a reusable pattern
- pin-timing hacks in examples
- ad hoc piece-specific state machines when a reusable pattern would do

Debug signals follow an opt-in pattern. The status LED and boot pulse on
`UfwLed` are gated behind `setDevMode(bool)` (wired up through
`UfwRuntimeConfig::devMode`) so finished pieces run dark by default. Future
debug features should follow this shape: off by default, one config flag to
enable, no effect on artistic content (animation pins, cue LEDs, phrase LEDs).

## Documentation Guidance

- User-facing docs should teach `sounds`, `phrases`, `cues`, and piece types in plain language.
- Technical docs should explain where to add new features and which layer owns them.
- Examples should stay short enough to serve as copyable templates.
