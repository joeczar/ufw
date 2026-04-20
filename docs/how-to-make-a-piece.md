# How To Make A Piece With Ufw

`Ufw` helps you build pieces out of:

- `sounds`: named audio files
- `phrases`: named LED patterns
- `cues`: sound-and-light pairs
- `pieces`: reusable playback objects that own the loop logic

You do not need to think about DFPlayer commands while writing a piece. Most of the time, you only edit the named lists near the top of an example.

## Building Blocks Ladder

Before jumping into a full piece, you can climb a ladder of small examples that
teach one idea at a time. Each rung is ~30–50 lines and the diff to the next
rung is usually a single line.

Audio foundation:

- `AudioPlayOne` - boot, play one track, stop
- `AudioStatusLed` - same, plus a status LED that follows playback (dev mode)

Phrase ladder (toward `RememberThisOneTime`):

- `PhraseOnce` - a `UfwPhrase` runs once at boot with no audio
- `PhraseWithSound` - the phrase runs alongside one track

Cue ladder (toward `IFuckingHateMen`):

- `OneCue` - one cue, plays once, stops
- `TwoCuesInOrder` - two cues advance automatically in order
- `CueShuffle` - one-line diff: `.shuffle()` instead of `.inOrder()`
- `CueRepeat` - one-line diff: `.repeatForever()` instead of `.stopWhenDone()`

`RememberThisOneTime` and `IFuckingHateMen` are the two full pieces the ladder
builds up to.

## Choosing A Runtime Shape

`UfwRuntime` has two forms. A piece should only declare the LEDs it actually
uses:

- `UfwRuntime(Serial1)` - audio only, no phrase/cue LEDs
- `UfwRuntime(Serial1, LED_PINS)` - audio + phrase/cue LEDs

In both forms the board's `LED_BUILTIN` is the status LED and only lights when
dev mode is on. You do not need to wire a separate status LED.

`AudioPlayOne` and `AudioStatusLed` are the audio-only rungs. All phrase and
cue examples use the second form.

## Dev Mode

The status LED (and the boot pulse) is a developer-facing signal and is off by
default. Finished pieces installed in the world run dark. To turn it on for
a debug sketch, pass `UfwRuntimeConfig{.devMode = true}` to `runtime.begin(...)`.
`AudioStatusLed` and `LibrarySmokeTest` are the examples that enable dev mode.

The examples are split into two zones:

- `edit here`: the part you are expected to change for a new piece
- `library-managed`: the part that wires the piece together and usually stays the same

The library now also provides a `runtime` helper for the repeated hardware setup.
That means examples do not need to spell out the USB baud, player baud, startup
volume, and LED boot setup every time.

## Two Kinds Of Pieces

### 1. Track piece
Use this when you have longer audio tracks, such as poems.

Example shape:
- list the named tracks
- list the LED phrase
- choose whether the tracks play `in order` or `shuffled`
- choose whether the piece `stops` or `repeats`

`RememberThisOneTime` is the track-piece example.

### 2. Cue piece
Use this when each sound has a matching LED.

Example shape:
- list the named cues
- each cue says which track to play and which LED to light
- choose whether the cues play `in order` or `shuffled`
- choose whether the piece `stops` or `repeats`

`IFuckingHateMen` is the cue-piece example.

## What To Edit

When making a new piece, start by copying an example and change only these parts:

1. The named sounds
   - example: `"poem1"`, `"meh"`, `"hate"`
2. The track numbers
   - example: `1`, `2`, `3`
3. The LED pins
   - example: `2`, `4`, `5`, `6`
4. The phrase steps or cue list
5. Whether the piece should shuffle, repeat, or stop

## What Not To Edit

Leave these alone unless you are changing the library itself:

- `UfwAudio`
- `UfwLed`
- `UfwFileTracker`
- `UfwTrackPiece`
- `UfwCuePiece`
- the basic `setup()` and `loop()` structure in the examples

In most examples:
- `setup()` asks the runtime to boot the hardware, then wires the piece together once and starts it
- `loop()` usually just calls `update()` so the library can keep the piece running

## Example: Track Piece

This style is for longer tracks plus one LED phrase at the start.

```cpp
UfwSound poems[] = {
  {"poem1", 1},
  {"poem2", 2},
  {"poem3", 3},
};

UfwPhraseStep rememberPhrase[] = {
  {2, 120},
  {4, 120},
  {5, 120},
  {6, 120},
  {7, 120},
  {8, 120},
  {9, 120},
};
```

This means:
- there are three poem tracks
- when a track starts, the LEDs run through the phrase once
- the piece class handles the shuffled order and stopping behavior

## Example: Cue Piece

This style is for short sound-and-light pairs.

```cpp
UfwCue cues[] = {
  {"i", 1, 2},
  {"fuh", 2, 4},
  {"kuh", 3, 5},
  {"ing", 4, 6},
};
```

This means:
- play track `1` when using cue `"i"` and light LED pin `2`
- play track `2` when using cue `"fuh"` and light LED pin `4`
- the piece class handles order, repeat behavior, and advancing to the next cue

## Workflow

1. Open a `Ufw` example in Arduino IDE.
2. Use `Save As...` to make your own sketch.
3. Find the `edit here` section near the top.
4. Rename the sounds, phrases, or cues.
5. Change the track numbers and LED pins.
6. Leave the library-managed section alone unless you are changing how the library works.
7. Upload to the board.
8. If the piece feels right, keep going. If not, change the lists and upload again.

## If Something Does Not Work

- If audio fails, check the DFPlayer track numbers first.
- If LEDs fail, check the LED pin numbers and wiring first.
- If the piece order feels wrong, check the shuffle/stop/repeat logic in the example you copied.

The easiest way to stay organized is:
- use `LedHardwareTest` when you want to test only the LED helper
- use `AudioPlayerTest` when you want to test only the DFPlayer helper
- use `RememberThisOneTime` as the starting point for poem-style pieces
- use `IFuckingHateMen` as the starting point for cue-style pieces

## LED Helper Notes

If you are working on the light behavior by itself, `LedHardwareTest` is the
reference example.

It demonstrates these built-in `UfwLed` animations:

- `startFillSweep(...)`
  - hard-edged sweep
- `startFadeSweep(...)`
  - soft moving window where LEDs fade toward their next brightness target
- `startBreathe(...)`
  - looping all-LED pulse
- `startBreatheOnce(...)`
  - one full in-and-out breath
- `startBreatheCount(...)`
  - a chosen number of breaths

For sweep-style animations:

- `maxLit` means the maximum number of LEDs active at once
- brightness can be one number such as `180`
- or a brightness profile array such as `{255, 120, 40}`
- profile arrays describe the moving window from newest `head` LED to oldest
  `tail` LED

All of these helpers are non-blocking. In a sketch, you start the animation
once, then keep calling `leds.update()` in `loop()`.
