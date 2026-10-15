# Sprite effect controllers (tentative)

The contiguous family begins at `0806A60C` and continues through `0806B2C4`.
Global `02000178` owns 24 controller slots, each `0x7C` bytes, starting at
`0x1C`; the used mask is at `0x18` and allocation is `0xBBC` bytes. The shared
view is `sprite_effect_controller.h`. Flags bit 8 selects a sequence drawable
(position `0x1C`) instead of a tile drawable (position `0x18`). Both drawable
variants have a flags word and active byte at offsets zero and four.

The queue at `6A60C` schedules at most six collider refreshes per call and gives
a scheduled slot a three-frame cooldown. Preserve its observed quirk: eligibility
bit 4 is read from the first slot for every occupied slot. The cooldown and
scheduled bit `0x100` belong to each individual slot. The refresh helper requires
both eligibility bit 4 and scheduled bit `0x100`; it optionally relocates the
collider before refreshing it. Collider geometry is 16 by 32 by 16.

`6A7D8` clears drawable flag 1 and resets the frame counter on entry, decodes the
packed animation command, refreshes the collider, then optionally adds velocity
to position (controller bit `0x80`). Sequence drawables cache frame weight and
clamp a zero animation delay to one; tile drawables compute the tile offset
from dimensions and resource base and allow zero delay. Playback can reverse,
loop, or hold the terminal frame. The shared playback helper records these
variant differences. Controller `lifetime` is a provisional name: it counts down
per update, but terminal handling waits for both sequence completion and zero
remaining time. It is not a hard deadline.

On completion, bit 1 releases the slot, removes an active drawable and frees its
heap block. Otherwise the controller enters state 4. A released controller gets
index -1, zero flags and an entry marker. Both callbacks return void; incidental
register state is not modeled as a return value.

Exact new progress currently: queue 136 bytes, refresh helper 56 bytes and
animation updater 836 bytes, totaling 1,028 bytes (about 0.0425 percentage
points), all with temporary matching concessions. Existing 268 bytes have typed
contracts without new credit. The updater and shared playback retain 24 hints
jointly minimized with `tools/depin.py --write`; none binds r7 or supplies an
instruction body. Integrator owns combined ROM validation. The later bounce and
other state bodies are still being recovered.
