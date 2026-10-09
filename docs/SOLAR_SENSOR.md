# Cartridge solar sensor: code and runtime evidence

The eight-function block `08243C10–08243EFC` controls the cartridge light sensor.
The low-level initialization (204 bytes), shutdown (76) and timer interrupt
(232) are newly matched independent C reconstructions, **512 new bytes**.
The five public wrappers were already matched. All use ordinary agbcc O2 with
interworking. Proposed names are in `symbols/proposed/solar.csv`; original
address labels remain unchanged.

## Function and state map

| Entry | Proposed role | Direct evidence |
|---|---|---|
| `08243C10` | Solar_InitTimer | Installs `08243D28` in interrupt-table slot 2, enables IE bit 6 and timer 3, configures cartridge GPIO |
| `08243CDC` | Solar_ShutdownTimer | Stops timer 3, restores `082141BC` handler, clears IE bit 6 and GPIO read enable |
| `08243D28` | Solar_TimerInterrupt | Pulses GPIO reset/clock, samples input pin 3 and stores half the interrupt counter as result |
| `08243E10` | Solar_Enable | Sets software enabled flag and invokes initialization |
| `08243E30` | Solar_Disable | Clears enabled flag and invokes shutdown if previously enabled |
| `08243E4C` | Solar_StartMeasurement | Enables if needed, clears phase/counter and sets measurement-active with result -1 |
| `08243E8C` | Solar_StopMeasurement | Disables, clears active flag and resets result to -1 |
| `08243EB8` | Solar_GetMeasurement | Returns result when active, otherwise -1 |

| RAM address | Observed meaning |
|---|---|
| `030035B0` | Interrupt phase, observed states 0, 1 and 2 |
| `030035B4` | Interrupt counter, incremented after each active handler pass |
| `030035B8` | Saved pin-3 mask, updated in phases 0/1 |
| `030035BC` | GPIO output shadow, with reset bit 1 and clock bit 0 |
| `030035C0` | Software enabled flag |
| `03006A34` | GPIO data snapshot taken during initialization |
| `03006A38` | Measurement-active flag |
| `03006A3C` | Latest GPIO input masked with 8 |
| `03006A48` | Signed measurement result; -1 means no available reading |
| `03006A4C` | Timer reload interval, initialized to 0x3128 (12,584 ticks) |

The purpose of initialized values at `03006A30`, `03006A40` and `03006A44`
is not established by this block alone; do not assign calibration meanings yet.
The init/shutdown routines temporarily disable IME and IE, then enable IME on
exit. They do not restore an arbitrary previous IME value. IE changes preserve
the other interrupt-enable bits. The init inline `u16` mask helper reproduces
the original explicit 16-bit normalization; a direct cast omitted two shifts.

## Protocol and integration

GPIO `080000C4/C6/C8` are hardware mappings, not relocatable ROM assets.
Initialization selects output bits 0–2 and leaves pin 3 as input. In phase 0,
the reset line stays high for the first ten counter values, then clears.
After counter 20 with input low, phase 1 begins and the counter resets. Phase 1
and phase 2 toggle the clock each interrupt. An input high in phase 1 stores
`counter >> 1` and advances to phase 2. After counter 511, phase and counter
reset for another conversion. The initial GPIO read precedes the output write,
so sample timing includes this pipeline.

This role agrees with mGBA's independent cartridge implementation:
[`_lightReadPins` at 143adf5aa2a60c6036810b0cd72445e13d74b0e6](https://github.com/mgba-emu/mgba/blob/143adf5aa2a60c6036810b0cd72445e13d74b0e6/src/gba/cart/gpio.c#L374).
It uses pin 1 for reset, pin 0 clock edges, pin 2 chip select and pin 3 for the
threshold output. The emulator marks part of its reset-edge behavior unverified.
No emulator source was copied into these C functions.

Caller `0822C198` accepts a nonnegative result only when byte 0x19 of the
object pointed to by `03002604` is 2, then stores result minus two into offset 4
of the object at `030053F8`. `0822C1C8` stops measurement and sets halfword
`030054BC`; `0822C1F0` restarts it and clears that halfword. These are integration
leads, not evidence that all nearby code belongs exclusively to the sensor.

## Reproducible harness observation

Use `tests/solar_sensor.txt` with the supplied late-game test save:

```sh
mkdir -p build/solar
BOKTAI3_SAV=tests/saves/ShinBok2.sav \
  tools/emu/harness baserom.gba tests/solar_sensor.txt build/solar
```

The script uses normal game input, changes only the emulator light source,
waits 120 frames per setting, snapshots the above state, and probes the result
store at `08243DD4` for 24 frames. It makes no RAM writes. The harness `lux`
command takes a raw sensor threshold, **not physical lux**.

| Emulator threshold | Snapshot at `03006A48` | Next probed result (`r1`) |
|---:|---:|---:|
| 20 | 20 | 21 |
| 80 | 80 | 80 |
| 180 | 181 | 180 |
| 232 | 232 | 233 |

These observed values support the sensor/result relationship and expose a
one-count timing variation; they do not prove a calibrated brightness scale.
This run used the corrected execution-probe harness described in
`docs/SCRIPT_TRACING.md`. Dumps, full traces, copied saves and screenshots remain
under ignored `build/`; only the reproducible script and findings are tracked.
Exact C checks and a complete original-ROM SHA-1 match separately establish
the code matches. Wireless RFU code is a distinct adjacent subsystem.
