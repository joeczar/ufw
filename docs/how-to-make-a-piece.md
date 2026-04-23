# How To Make A Piece With Ufw

`Ufw` helps you build pieces out of:

- `sounds`: named audio files
- `phrases`: named LED patterns
- `cues`: sound-and-light pairs
- `pieces`: reusable playback objects that own the loop logic

You do not need to think about DFPlayer commands while writing a piece. Most of the time, you only edit the named lists near the top of an example.

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
- use `MicSignalTest` when you want to inspect an analog mic signal on Serial
  before turning it into a reusable trigger
- use `RememberThisOneTime` as the starting point for poem-style pieces
- use `IFuckingHateMen` as the starting point for cue-style pieces

## Mic Helper Notes

If you are working on microphone input by itself, `MicSignalTest` is the
reference example.

It demonstrates the low-level `UfwMic` helper:

```cpp
UfwMic mic(26, 100);

void setup() {
  Serial.begin(115200);
  mic.begin(12);
}

void loop() {
  if (mic.update()) {
    Serial.println(mic.peakToPeak());
  }
}
```

This means:
- `26` is the analog input pin
- `100` is the measurement window in milliseconds
- `update()` should be called as often as possible in `loop()`
- when `update()` returns `true`, one full window is ready to inspect
- `peakToPeak()` is the easiest loudness number to tune against first
- `center()` helps you see the mic's quiet bias level

## LED Helper Notes

If you are working on the light behavior by itself, `LedHardwareTest` is the
reference example.

It demonstrates these built-in `UfwLed` animations:

- `startFillSweep(...)`
  - hard-edged sweep
- `startFadeSweep(...)`
  - soft moving window where LEDs fade toward their next brightness target
- `startComet(...)`
  - moving bright head with a dimmer tail
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
