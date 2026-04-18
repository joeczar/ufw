# Pico W to DFPlayer Pro Wiring

This wiring assumes:

- `Raspberry Pi Pico W`
- `DFRobot DFR0768 / DFPlayer Pro`
- one small 2-wire test speaker connected directly to a single amplified output channel
- the Pico W is powered over USB, so `VBUS` is available as a 5 V source for the audio module

## Recommended Wiring

| Pico W | DFPlayer Pro | Notes |
| --- | --- | --- |
| `VBUS` (pin 40) | `VIN` | Power the module from USB 5 V for better speaker headroom |
| `GND` | `GND` | Common ground |
| `GP0` / `UART0 TX` (pin 1) | `RX` | Pico transmits commands to the module |
| `GP1` / `UART0 RX` (pin 2) | `TX` | Pico receives responses from the module |
| Speaker lead A | `L+` | Left speaker output |
| Speaker lead B | `L-` | Left speaker output return |

## Important Notes

- `L+` and `L-` are a differential speaker output. Do **not** connect either speaker lead to `GND`.
- If you want to use an amplified speaker or external amp instead of a raw speaker, use `DACL` plus `GND` as a line-level style output instead of `L+` / `L-`.
- The DFPlayer Pro also has `R+` and `R-` for a second speaker channel if you want stereo later.
- If the module resets or crackles at higher volume, keep the same UART wiring but use a sturdier 5 V supply for `VIN` and share `GND` with the Pico.

## Local Reference Files

- DFPlayer pinout image: [dfplayer-pro-pinout.webp](/Users/hailmary/Code/personal/ufw/reference/dfplayer-pro-pinout.webp)
- DFPlayer schematic PDF: [dfplayer-pro-schematic.pdf](/Users/hailmary/Code/personal/ufw/reference/dfplayer-pro-schematic.pdf)
- DFPlayer schematic preview: [dfplayer-pro-schematic.pdf.png](/Users/hailmary/Code/personal/ufw/reference/dfplayer-pro-schematic.pdf.png)
- Pico W pinout PDF: [pico-w-pinout.pdf](/Users/hailmary/Code/personal/ufw/reference/pico-w-pinout.pdf)
- Pico W pinout preview: [pico-w-pinout.pdf.png](/Users/hailmary/Code/personal/ufw/reference/pico-w-pinout.pdf.png)
- Generated wiring schematic: [pico-w-dfplayer-speaker.svg](/Users/hailmary/Code/personal/ufw/schematic/pico-w-dfplayer-speaker.svg)
- Generated wiring preview: [pico-w-dfplayer-speaker-full.png](/Users/hailmary/Code/personal/ufw/schematic/pico-w-dfplayer-speaker-full.png)

## Source Links

- DFRobot wiki: [wiki.dfrobot.com/dfr0768](https://wiki.dfrobot.com/dfr0768/)
- Raspberry Pi Pico W pinout: [datasheets.raspberrypi.com/picow/PicoW-A4-Pinout.pdf](https://datasheets.raspberrypi.com/picow/PicoW-A4-Pinout.pdf)
