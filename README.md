# Ufw Arduino Library

This repo is now a standard Arduino library named `Ufw`.

The goal is to give you one reusable library that can be installed once and updated in one place while multiple sketches keep using it.

## Design Docs

- Technical design: [docs/technical-design.md](docs/technical-design.md)
- User guide: [docs/how-to-make-a-piece.md](docs/how-to-make-a-piece.md)

## Install For Arduino IDE

Clone or copy this repo into your Arduino sketchbook's `libraries` folder as `Ufw`:

```text
~/Documents/Arduino/libraries/Ufw
```

After restarting Arduino IDE, the examples should appear under:

`File -> Examples -> Ufw`

## Update Workflow

The clean update story is:

1. Keep one source of truth in this repo.
2. Install it once as the `Ufw` library.
3. When you change the library, update that one installed copy.
4. All sketches using `#include <Ufw.h>` get the new behavior.

If the installed copy is a git clone, updating is just:

```bash
git pull
```

## Library Layout

```text
Ufw/
  docs/
    technical-design.md
    how-to-make-a-piece.md
  library.properties
  src/
    Ufw.h
    UfwAudio.h
    UfwAudio.cpp
    UfwTypes.h
    UfwLed.h
    UfwLed.cpp
  examples/
    FirstLed/
      FirstLed.ino
    LedSetPin/
      LedSetPin.ino
    LedFillSweep/
      LedFillSweep.ino
    LedFadeSweep/
      LedFadeSweep.ino
    LedBreathe/
      LedBreathe.ino
    LedHardwareTest/
      LedHardwareTest.ino
    AudioPlayerTest/
      AudioPlayerTest.ino
    AudioPlayOne/
      AudioPlayOne.ino
    AudioStatusLed/
      AudioStatusLed.ino
    PhraseOnce/
      PhraseOnce.ino
    PhraseWithSound/
      PhraseWithSound.ino
    OneCue/
      OneCue.ino
    TwoCuesInOrder/
      TwoCuesInOrder.ino
    CueShuffle/
      CueShuffle.ino
    CueRepeat/
      CueRepeat.ino
    LibrarySmokeTest/
      LibrarySmokeTest.ino
    RememberThisOneTime/
      RememberThisOneTime.ino
    IFuckingHateMen/
      IFuckingHateMen.ino
```

## Example Workflow

1. Install `Ufw` into the Arduino `libraries` folder.
2. Open Arduino IDE.
3. Open an example from `File -> Examples -> Ufw`.
4. Use `Save As...` to create a new sketch.
5. Edit only the sketch, not the library internals, unless you want to change shared behavior.

## Notes

- `FirstLed` is the smallest single-LED sanity-check sketch.
- `LedSetPin` is the first direct `UfwLed` example. It shows how to turn on a
  specific LED pin.
- `LedFillSweep` shows how to start a sweep and how `maxLit` limits how many
  LEDs can be on at one time.
- `LedFadeSweep` shows how to use a brightness profile with `startFadeSweep(...)`.
- `LedBreathe` shows the `startBreatheOnce(...)`, `startBreatheCount(...)`, and
  `startBreathe(...)` helpers together.
- `LedHardwareTest` is the full `UfwLed` demo and hardware check.
  It exercises every configured LED pin and every built-in LED helper.
- `AudioPlayerTest` is the minimal `runtime + track piece` DFPlayer bench test.
- `AudioPlayOne` is the smallest audio example: boot, play one track, stop.
- `AudioStatusLed` adds `LED_BUILTIN` mirroring playback (enables dev mode).
- `PhraseOnce` runs a `UfwPhrase` once at boot with no audio.
- `PhraseWithSound` runs a phrase alongside one track (the atom `RememberThisOneTime` scales up).
- `OneCue` is the smallest possible `UfwCuePiece`: one sound, one LED, plays once.
- `TwoCuesInOrder` is the same idea with two cues advancing automatically.
- `CueShuffle` is the one-line diff that demonstrates `.shuffle()`.
- `CueRepeat` is the one-line diff that demonstrates `.repeatForever()`.
- `LibrarySmokeTest` is a quick LED + audio integration example (dev mode on).
- `RememberThisOneTime` is the canonical track-based piece example.
- `IFuckingHateMen` is the canonical cue-based piece example.
- `UfwAudio`, `UfwLed`, and `UfwFileTracker` are implementation-level helpers. The examples and docs are the main authoring surface for new pieces.

## Choosing A Runtime Shape

`UfwRuntime` has two forms. Pick the one that matches your sketch:

```cpp
// Audio only. No phrase/cue LEDs. LED_BUILTIN is the status indicator.
UfwRuntime runtime(Serial1);

// Audio + animation LEDs (phrase or cue pins). LED_BUILTIN is still the
// status indicator.
UfwRuntime runtime(Serial1, LED_PINS);
```

In both forms the board's built-in LED serves as the status LED, and is only
driven when dev mode is on (see below).

## Dev Mode (Status LED)

The status LED (and the boot pulse) is a developer-facing signal, so it is off
by default. Finished pieces installed in the world run dark.

To turn it on in your own sketch, pass a config to `runtime.begin(...)`:

```cpp
UfwRuntimeConfig config;
config.devMode = true;
runtime.begin("My Piece", config);
```

`AudioStatusLed` and `LibrarySmokeTest` are the examples that enable dev mode.

## LED Animation Surface

If you are working directly with `UfwLed`, the current built-in animation helpers are:

- `startFillSweep(...)`
- `startFadeSweep(...)`
- `startBreathe(...)`
- `startBreatheOnce(...)`
- `startBreatheCount(...)`

The LED helpers are all non-blocking. Start an animation once, then keep calling
`leds.update()` from `loop()`.

For sweep-style animations:

- `maxLit` controls how many LEDs may be active at once
- brightness can be one number or a brightness profile array
- brightness profile arrays run from newest `head` LED to oldest `tail` LED
