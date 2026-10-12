# Animation controller matching batch

The October 12 batch advances code coverage from **13.779% to 14.564%**:
**+0.785 percentage points**, 55 exact functions / 18,952 tracked code bytes.
Of those, 37 functions / 14,680 tracked bytes belong to this controller; the
other 18 are the earlier orbit/spawn effect family in the same integration.
The roles below are provisional descriptions of the C behavior, not an identified
character or a claim that this is the game's entire animation system.

## What now works in C

Two contiguous groups are fully matched:

- **Child effects and state reset** (`081E5608–081E692C`, 12 functions): consume
  child flags; change sprite effects; delay and trigger mode changes; advance
  animation; reset scale and position-related fields; restore the default phase;
  release an effect handle; reset all 16 active child callbacks.
- **Animation states and transitions** (`081E78F4–081E9BA4`, 17 functions): invoke
  a previous-state callback, advance frame timing and flip flags, rotate or scale
  sprites, release finished effects, blink, emit a radial burst, and reset children.

Additional matched helpers arm an idle transition at a resting position, restore
its previous phase, grow its scale, dispatch modes/phases, check resource lifetime,
and release the controller and its children.

| Readable role | Functions |
|---|---|
| Grow scale toward 127; arm idle transition; restore interrupted phase | `081E41B0`, `081E41EC`, `081E43E8` |
| Reset child; delayed mode transition; release effect; select child sprite | `081E5608`, `081E5648`, `081E574C`, `081E57A8` |
| Rotate and advance animation | `081E57EC` |
| Initialize/reset scale, position, animation, and effect state | `081E59CC`, `081E5C4C`, `081E5ED4`, `081E6150`, `081E63C4`, `081E6604` |
| Hide controller and reset active children | `081E682C` |
| Advance animation with previous-state callback | `081E78F4`, `081E7AA0`, `081E7C3C`, `081E99EC` |
| Initialize one-shot animation/effect | `081E7DFC` |
| Blink, emit four effects and eight radial particles, then reset | `081E7F80` |
| Rotating animation states 2–9 | `081E8374`, `081E85B8`, `081E87FC`, `081E8A40`, `081E8C84`, `081E8EA8`, `081E90EC`, `081E9330` |
| Finish animation and reset active children | `081E9574` |
| Rotate while growing/shrinking scale | `081E97D4`, `081E98E0` |
| Copy saved position and dispatch phase; prepare and dispatch mode | `081E9C9C`, `081E9D10` |
| Check referenced resources; update callbacks; release controller | `081E9D3C`, `081E9D84`, `081E9DE4` |

## Reusable layout and matching details

This controller embeds its principal sprite at `DDC`, animation at `E24`, and
motion-related state at `E34`. Animation entries pack a sprite frame in bits
6 and above, flips in bits 4–5, and a timing factor in bits 0–3. The timer and
frame paths support forward/reverse playback and end-frame holding. A 4-bit mode
and 12-bit phase share the halfword at `F5A`; `F64` includes the state-entry flag.
Sixteen child records have stride `DC`, starting at `1C`; their callback and
control byte are at offsets `AC` and `B1` within each child.

These observed layouts are expressed locally in the matching sources. Consolidating
them into headers and identifying the actor from resource IDs remain follow-up
work, not prerequisites for accepting exact matches.

Register hints solved flag/flip operands, timer truncation, frame-factor loads,
and a few persistent locals. `MATCHING_CONCESSION: register-pinning` marks these
files. Two cleanup paths retain an otherwise unused zero-register initialization
that the original compiler emitted; those are explicitly marked as concessions.
No handwritten instruction bodies are used. The native report separates marked
sources without reducing their exact credit.

## Validation and remaining work

Every promoted function passed its fast exact check, and the combined build
printed `build/boktai3.gba: OK`. The comparison includes resolved calls/literals;
retained trailing assembly bytes remain in the ROM. Tracked progress includes
alignment and pools under the existing reporting convention.

Still unfinished: the idle wait callback `081E42C4` (retained near-match), the
larger movement paths `081E692C`, `081E6E0C`, `081E728C`, child visibility dispatcher
`081E9BA4`, and controller initialization. The separate WIP branch preserves the
current drafts and allocation experiments as source only. The old 1,060-byte
spawn family (`08160EA4` and siblings) remains nonmatching and gets no exact credit.
