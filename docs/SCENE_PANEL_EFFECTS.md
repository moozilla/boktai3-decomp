# Scene panel particles and time palettes

`08057960–08058130` is an earlier group of consumers of the same large object
viewed by `scene_panel.h`. The additional layout is in
`include/scene_panel_effects.h`; its public functions take `struct ScenePanel *`
and cast to the local effects view. `ScenePanel` remains a tentative identity.
These are static contracts, without claims of observed gameplay coverage.

The effect state/frame halfwords at `0x4FC/0x4FE` are separate from the state
callback at `0x250` and its counter at `0x22E`. The state setter stores the new
state and clears its counter. An optional script resource ID at `0x17AC` is
called with one unchanged argument word through the standard argument
count/pointer descriptor. The descriptor sets its low count halfword to one
and preserves the upper halfword, as in the original.

The particle resource is at `0x560`. Thirty-two `0x34` entries begin at `0x564`;
shared active/cursor bytes are at `0xBE4/0xBE5`. Each particle has flags at zero,
render-mode byte at `0x0F`, 8-byte position at `0x18`, 8-byte velocity at `0x28`,
and active/age/lifetime bytes at `0x30/0x31/0x32`. Insertion overwrites the
current cyclic slot, clears hidden flag 1, copies the supplied position and
velocity, selects animation frame 2, stores the lifetime and render mode,
clears age, and marks the entry and pool active. The cursor wraps after 31.
There is no inactive-slot search or capacity rejection.

While the pool is active, update visits every slot. An active particle hides
and deactivates when its age is already at least its lifetime. Otherwise it
adds velocity to position, selects animation frame 3 at age 6, then increments
age. The shared active byte becomes whether any particle took that motion
path. Initialization finds resource `0x1C1E`, clears the shared active byte,
initializes every sprite with helper mode 1, scale `(-4, -4)`, and render helper
mode 1, then clears each entry's active byte. It does not explicitly reset the
cursor. Destruction removes all thirty-two sprites.

The panel has a 256-color buffer at `0x20`, arranged as sixteen rows of sixteen
RGB15 colors. Three palette source pointers are at `0x220`; source and target
selection bytes are at `0x22C/0x22D`. The current time phase is at `0xC0B`.
Palette blend frame/active and channel-dirty halfwords are at
`0x142C/0x142E/0x1430`.

Initialization reads three optional `A` values, defaulting to `0x2365`,
`0xF80C`, and `0xA344`. It narrows each resource name to sixteen bits, looks it
up with extension hash `0x92B3`, and stores the returned address plus `0x14`.
It preserves the original absence of a null check. It samples the time phase,
applies the selection below, then clears blend frame/active and copies the
target source into the full 256-color buffer and the first three staging rows
at `gUnk_03004FC0`. It initializes channels 0–2 to 0, 1, and 2 and clears the
state callback.

| Time phase | Source | Target | Starts a blend |
| --- | --- | --- | --- |
| 0 | 2 | 2 | No |
| 1 | 2 | 0 | Yes |
| 2 | 0 | 0 | No |
| 3 | 0 | 1 | Yes |
| 4 | 1 | 2 | Yes |
| 5 | 2 | 2 | No |

Starting a blend clears its frame and sets active. The other cases preserve
an existing blend's counter and active field. The phase getter at `08228DB8`
returns full-word values 0–5; this updater compares the full return value
before narrowing the stored phase byte.

A blend update marks channels dirty and increments the sixteen-bit frame.
Frames 1–63 blend every RGB channel independently with weights `64 - frame`
and `frame`, using a signed shift right by six. At frame 64 the target source
is copied directly and active clears. The main sprite at `0x254` is eligible
for palette animation when its active halfword at relative `0x68` is nonzero
and hidden flag 1 is clear. Its graphics are at relative `0x2C`, palette
pointer at `0x38`, sixteen-color working buffer at `0x48`, cached palette frame
at `0x6A`, and palette-cycle counter at `0x6E`.

The cycle counter advances only when the visibility predicate succeeds and
wraps after 129. It selects frames 0 for 0–29, 1 for 30–39, 2 for 40–49, 3 for
50–109, 2 for 110–119, and 1 for 120–129. The palette bank is
`0x203 + selection * 4 + frame`. During a blend, the updater also blends the
sprite's sixteen colors from the corresponding source/target banks in
`gUnk_030042E4`, installs its working-buffer pointer, and leaves the cached
palette frame unchanged. At blend completion it updates the cache and selects
the target bank. With no active blend, it reselects a bank only when that
cached frame changes.

The dispatcher calls a nonnull state callback, updates the time palette, and
then uploads the first three selected channel rows into `gUnk_03004FC0` when
channels are dirty. Each row is a 32-byte CpuSet copy. It clears dirty after
the uploads. This is a staging-buffer update, without an asserted direct VRAM
write.

All twelve functions match their complete reviewed extents. Seven are newly
recovered, totaling 1,792 bytes, a 0.0742 percentage-point gain; five existing
matching functions totaling 208 bytes now share these contracts. Remaining
register bindings and empty barriers are temporary matching concessions,
minimized before commit. The explicit palette-loop scratch structure records
persistent loop state; no handwritten instruction body or explicit r7 binding
is used. Combined ROM validation belongs to the integrator's batch.

## Background effect updater

`08058700–08058AF4` is an additional consumer of the same panel. It takes the
panel and four tilemap positioning arguments. It controls display-shadow bit
`0x200`, the alpha register at `0x04000052`, and tilemap layer 1. Its palette
source at `0x4D8` is copied from offset `0x20` into the sixteen-color staging
area at `0x03005140` when `gUnk_02000710->unk618` is nonpositive.

When that global field is positive, the shared `mode` counter controls a
fade/shake path. A zero counter clears the display-shadow bit. Values above
31 use alpha 16; values above 35 additionally sample a shake offset in 0–7,
while 32–35 use offset 5. Values at most 31 use half the counter as alpha and
decrease a positive shake offset on counters divisible by four. The updater
loads the tilemap with the shake added to screen Y, then decrements the counter.

With a nonpositive global field and `effectPhase` at `0x1378` equal to zero,
the counter cycles through 0–63. Its triangular contribution gives alpha
12–16, with a freshly sampled shake offset every update. Every fourth frame
it submits a packed center offset by 64 in X and Z, and a velocity of 300 on
all three axes, to `0806A160` with the unchanged remaining arguments.

For a nonzero effect phase, a nonzero counter produces one local panel
particle per update. The packed center at `0x510` receives three independent
random offsets in -250–249. Its vertical component is interpreted as signed
sixteen-bit; the horizontal components use their unsigned sixteen-bit values
before wrapping the final result. Velocity is `(0, 16–47, 0)`, lifetime is
8–15, and render mode is 1. The fourth random index remains local until the
fifth sample advances the shared index, preserving the original write order.
The packed word view preserves the original untouched padding bits and word
writes. Alpha is the counter divided by four; shake rises toward seven every
eighth frame. The final tilemap call is followed by a fresh counter access;
reaching zero invokes `08057978` on the global panel with script argument 9.

This updater matches its complete 1,012-byte extent, an additional 0.0419
percentage points. Its temporary hints were minimized from 70 to 17; the
function has no explicit r7 binding. Its additional layout comes from the
integrator's `scene_panel.h` view, rather than a competing structure definition.

## Overlay tile renderer

`08058BD8–08058FF8` first calls the overlay-state helper at `08058B88`, then
records the four supplied layer-position arguments in `gUnk_03004CC0`. The
additional layer/resource views are in `scene_panel_overlay.h`. The layer has
signed width/height at `0x18/0x1A`, its 2-by-2 tile table at `0x24`, selected
block map at `0x28`, and output pointer at `0x2C`. The renderer doubles the
stored dimensions for its world-tile bounds checks.

It projects `overlayOrigin` at panel offset `0x500`: projected X is
`3 * (x - z) / 32`, while projected Y/Z are `3 * (x + z) / 32` minus/plus
`3 * y / 32`. Each division truncates toward zero. Camera offsets are added
before division by eight; the resulting Y center is rounded down to an even
value. Supplied tile offsets are then subtracted from the centers.

The map resource has a 24-byte header. Its fourth word is a relative offset
to 12-byte map entries, whose third word is a relative block-map offset.
Relocation occurs in local header/entry copies. The four quadrant slots use
the observed order 0, 2, 3, 1: left/top, right/top, left/bottom, right/bottom.
The left/top extents are seven columns and six rows; right/bottom extend seven
columns and four rows. Maxima are exclusive. Only minima clamp to zero; an
additional early rejection tests negative maxima or minima above 31.

For each slot, palette mode zero or effect state outside 10/14 selects level
5. Otherwise the referenced subject's state byte at `0xD3` selects level 0
when it equals 4, and level 5 otherwise. The requested level is stored in the
four bytes at `0x4E4`; comparison with the current bytes at `0x4E8` suppresses
unchanged quadrants. A transition to zero requests one sound 245 after all
slots. `overlayChanged` at `0x132E` is cleared at entry to this process.

Level 5 clears pairs of output rows. Other levels select map entry
`level + 1`. The block lookup uses the world tile's 32-by-32 block coordinates,
a stride of stored width divided by sixteen, and a 16-by-16 array of 2-by-2
tile references within each block. The tile reference selects four halfwords
from the tile table; column parity chooses the left or right pair. World
coordinates outside the doubled dimensions produce a cleared pair instead.
The output column and first row each mask with 31. The second row uses
`maskedRow + 1` without an additional wrap mask, preserving the original.

The renderer matches all 1,056 reviewed bytes, another 0.0437 percentage
points. Temporary register bindings and empty barriers were minimized from
142 to 48. The remaining scratch output is defined by an empty asm output
before its input uses; the function has no explicit r7 binding. The literal
RAM alias names `gUnk_03004CC0` for the combined build's address check.
