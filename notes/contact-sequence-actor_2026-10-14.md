# Sequence contact actor pool, October 14

Tentative family name for **08045D18–08046880**, continuing the preceding
contact/swarm, motion and projected-display subsections in ROM order.

Exact progress rises **23.496% → 23.595%**, **+0.0992 percentage points**
(+2,396 bytes, thirteen new functions). There are 6,427 / 11,470 exact
functions and 569,910 / 2,415,354 exact bytes. Register-pinned code is
**4.256%** of total code; code without pinning is **19.339%**.

| Address | Function | Full bytes | Status |
| --- | --- | ---: | --- |
| 08045D54 | ApplyHealthContact | 180 | Exact, one retained hint |
| 08045E08 | Initialize actor | 268 | Exact, no hints |
| 08045F14 | Deactivate | 48 | Exact, one retained hint |
| 08045F48 | UpdateAppearing | 384 | Exact, ten retained hints |
| 080460C8 | UpdateActive | 264 | Exact, ten retained hints |
| 080461D0 | UpdateDying | 416 | Guarded draft, fuzzy 97.0%; no exact credit |
| 08046370 | Update actor | 384 | Exact, eleven retained hints |
| 0804650C | Update pool | 192 | Exact, depinned completely |
| 080465CC | Destroy pool | 40 | Exact, no hints |
| 080465F4 | Initialize pool | 104 | Exact, no hints |
| 0804665C | Create pool | 100 | Exact, no hints |
| 080466C0 | FindInactiveActor | 36 | Exact, no hints |
| 080466E4 | SpawnActor | 336 | Exact, eight retained hints |
| 08046834 | BeginAllDeaths | 60 | Exact, no hints |

Eight existing helpers/lifecycle/accessor functions were typed and checked,
without claiming new credit. Seven new functions use no register hints;
six retain temporary matching concessions. Every hinted exact candidate went
through `tools/depin.py --write` before acceptance. No handwritten instruction
bodies were introduced. The hidden ClearFlags helper at 08045D20 retains its
file-local alias because the generated inventory has no function boundary there.

## Recovered layout and behavior

`include/contact_sequence_actor.h` recovers the full 0x118 actor and 0x918
list-8 task. Eight actors start at 0x34; owner fade controls are at 0x8F4/0x8F6
and the 32-byte palette is at 0x8F8. The existing shared contact collider,
graphics, sprite and packed-sequence layouts fit these consumers directly.

The state table gUnk_086059A4 contains inactive, appearing, active and dying
callbacks. State changes reset the frame counter and set a consumable entry
flag. Appearance starts a variant-selected sequence, plays song 0x3B8 at
sequence index 3, and enters active state 2 when playback ends. Active playback
refreshes the resource collider. Packed commands encode frame, flip flags and
frame weight; duration is recomputed from speed with a minimum of one.

The health collider at 0x6C has flags 0x4005; the resource collider at 0x18 has
flags 0x2001. Bounds are 80/128/80 with offset 0/128/0. Accepted health contacts
subtract the other collider's cost, start a ten-frame cooldown and eight-frame
flash, emit effects, and enter death state 3 at depleted health. The per-frame
update uses hit tile 0x192 during cooldown, disables the health collider through
bit 4 when the state requests it, and clears its cost after processing. Flash
jitter uses an RNG direction and sine samples scaled by remaining flash time,
with signed division by 4096 truncating toward zero; its final frame restores
the unjittered sprite position.

Owner +0x18 initializes actor health. Owner +0x20 is a distinct resource
collider parameter, passed with 50 to sub_0821FF50; its interpretation remains
tentative (`resourceValue`). Owner +0x24 supplies the health-contact reaction
value. Resource lookup uses hashes 922E/C5E2 and actor graphics use 4147.

Spawn finds an inactive slot, computes the terrain cell from signed x/z >> 8,
uses a modified cell when available, and sets y from the cell's height nibble.
Slope kinds 1 and 2 subtract the low byte of z and x, respectively. It places
both colliders, initializes health and variant, starts appearance, and marks
the slot active. Null pools/full pools return -1. BeginAllDeaths moves every
active actor in state 0–2 to state 3. The active-count accessor returns zero
for null; the pool's count includes state 2 only.

The palette oscillates between levels 0 and 32 whenever state-2 count is
nonzero, blending five colors from tile-bank offsets 0x2C00 and 0x2CA0.
Initialization copies eight words from the first bank into the owner palette.

Static callers include sub_08029A78 (spawn using world position and variant
x + z * 3), owner initialization near 0802C5A0, and multiple scene-controller
calls to BeginAllDeaths. This is static evidence; no new emulator coverage or
gameplay identity is claimed.

## Retained death candidate

The guarded handler emits two randomized effects and a particle burst on state
entry. Each effect consumes three RNG values; the counter is written after each
triplet. If health is depleted and the owner script key is nonzero, it calls the
script with three signed coordinates, then plays song 0x3BA. It always requests
health-collider disabling and deactivates the slot.

It differs by six bytes at equal 416-byte extent: scratch-register choices for
signed health/y loads and the mask reload used to set script argument count.
`tools/regdiag.py` identifies the count mask reload; removing individual hints,
changing argument qualifiers and clobber placement did not resolve the residue.
The draft remains under NONMATCHING with its original INCLUDE_ASM fallback and
recorded fuzzy 97.0%; it does not contribute exact bytes.

## Validation

The combined build printed **`build/boktai3.gba: OK`** in
`build/contact-sequence-combined.log`. Twenty-five post-promotion checks passed
at reviewed full extents: thirteen new matches, eight typed existing functions,
and four renamed scene-owner callers. The guarded death handler was separately
checked with `--nonmatching --record` at its full 416-byte extent. Formatting,
ROM inventory, named RAM literals and decoded actor-registry checks passed.

The next contiguous subsection begins at **08046880** with another state actor.
