# Ufw Arduino Library

This repo is now a standard Arduino library named `Ufw`.

The goal is to give you one reusable library that can be installed once and updated in one place while multiple sketches keep using it.

## Design Docs

- Technical design: [docs/technical-design.md](/Users/hailmary/Code/personal/ufw/docs/technical-design.md)
- User guide: [docs/how-to-make-a-piece.md](/Users/hailmary/Code/personal/ufw/docs/how-to-make-a-piece.md)

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
    UfwMic.h
    UfwMic.cpp
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
    LedComet/
      LedComet.ino
    LedBreathe/
      LedBreathe.ino
    LedHardwareTest/
      LedHardwareTest.ino
    MicSignalTest/
      MicSignalTest.ino
    AudioPlayerTest/
      AudioPlayerTest.ino
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
- `LedComet` shows how to use `startComet(...)`.
- `LedBreathe` shows the `startBreatheOnce(...)`, `startBreatheCount(...)`, and
  `startBreathe(...)` helpers together.
- `LedHardwareTest` is the full `UfwLed` demo and hardware check.
  It exercises every configured LED pin and every built-in LED helper.
- `MicSignalTest` is a Pico W analog microphone bench test.
  It prints rolling Serial measurements for a mic on `GP26` so you can inspect
  quiet bias, min/max swing, and peak-to-peak level before building triggers.
- `AudioPlayerTest` is the minimal `runtime + track piece` DFPlayer bench test.
- `LibrarySmokeTest` is a quick LED + audio integration example.
- `RememberThisOneTime` is the canonical track-based piece example.
- `IFuckingHateMen` is the canonical cue-based piece example.
- `UfwAudio`, `UfwLed`, `UfwMic`, and `UfwFileTracker` are implementation-level
  helpers. The examples and docs are the main authoring surface for new pieces.

## LED Animation Surface

If you are working directly with `UfwLed`, the current built-in animation helpers are:

- `startFillSweep(...)`
- `startFadeSweep(...)`
- `startComet(...)`
- `startBreathe(...)`
- `startBreatheOnce(...)`
- `startBreatheCount(...)`

The LED helpers are all non-blocking. Start an animation once, then keep calling
`leds.update()` from `loop()`.

For sweep-style animations:

- `maxLit` controls how many LEDs may be active at once
- brightness can be one number or a brightness profile array
- brightness profile arrays run from newest `head` LED to oldest `tail` LED
