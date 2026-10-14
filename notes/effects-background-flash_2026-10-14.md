# Transient sprite effects and BG1 flash controller, October 14

Continue in ROM order through **08047338–080476F4** and
**080476F4–08047BB4**. Both subsections are now fully exact.

Exact code rises **23.671% → 23.746%**, **+0.0757 percentage points**
(+1,828 bytes, thirteen new functions). There are **6,453 / 11,471 functions**
and **573,558 / 2,415,354 exact bytes**. Code without register pinning is
**19.415%**; Register-pinned is **4.332%**, included above. The seventy-two
existing guarded drafts remain excluded from exact credit.

A previously hidden, independent 252-byte script wrapper at **080475F8** is
registered in build/progress boundary metadata. Its own push, final spawn call,
pop/bx and alignment establish its extent. This adds one function to the
inventory, with no change to the total code-byte denominator. No generated
assembly was edited. The hidden wrapper and existing hidden BG accessors
retain file-local aliases when the generated disassembly has no function-start
label for name promotion.

| Family | New exact functions | New bytes | Existing functions typed |
| --- | ---: | ---: | ---: |
| Transient sprite effects | 5 | 888 | 1 |
| BG1 flash/scroll controller | 8 | 940 | 11 |

Seven new functions use no register hints; six retain minimized concessions:
effect update (three hints), effect initialization (nine), BG scrolling (two),
appearance (one), disappearance (one) and BG initialization (two).
Every hinted exact candidate was minimized with `tools/depin.py --write`.
No handwritten instruction bodies were used.

## Transient effects

`include/transient_sprite_effects.h` describes a **0x598 list-10 singleton**
at gUnk_020000A0. Sixteen **0x58 slots** start at 0x18. Each slot has active and
option bytes, age, damping delay and multiplier, a velocity vector, graphics
at 0x10 and sprite at 0x2C. A union exposes both packed initialization fields
and ordinary signed coordinates without changing their eight-byte layout.

Initialization loads resource **8639** into every slot, initializes its sprite,
sets rendering priority one and writes a packed zero position. Packed z's
padding halfword remains unspecified, as in the original. The readable local
union initializes the x/y word completely and the z field separately. The loop
index is explicitly saved/restored around both calls; a hint in a caller-clobbered
register alone would not be sufficient preservation.

Spawn creates the singleton on demand, finds an inactive slot and returns -1
if allocation fails or the pool is full. Tile selection is **kind + 153**.
An optional velocity is copied, otherwise filled with zero. Option mask 0x1 maps
to sprite flag 0x200; mask 0x2 maps to flag 0x10, which suppresses depth movement.
With option mask 0x4 set, the rendering-priority byte receives bits C000 shifted
by twelve; otherwise it is one. The interpretation of these encoded priority
values remains tentative. Age starts at zero and damping parameters truncate
to halfwords, matching the original.

Once age reaches the delay, each velocity component is multiplied by the
halfword multiplier and divided by 256, truncating signed results toward zero.
Position advances each update; frame is age >> 2. The slot is deactivated and
its sprite removed after age exceeds 24. Destroy removes active sprites and
clears the singleton. The constructor retains its original context argument,
although the initializer does not consume it.

The hidden script wrapper reads keywords **t=kind**, **f=options**, **p=position**,
**s=velocity**, **d=delay/multiplier**. Position and velocity default to zero;
damping defaults to **24/256**. The initializer's packed vector and script
vectors have defined x/y/z fields with unspecified padding. Static calls from
the preceding death handler use kind zero, options 1004, no velocity and damping
256/256; these retain the observed effect behavior.

New full extents: 08047338 = 184; 080473F0 = 52; 08047424 = 124;
080474E4 = 276; 080475F8 = 252. Existing constructor 080474A0 = 68.

## BG1 flash and scrolling

`include/background_flash.h` recovers a **0x48 list-9 task**. Its overall frame
counter is at 0x18, state/entry/page at 0x1C/0x1D/0x1E, effect sources at
0x20/0x24, state frame at 0x28, current/target speeds at 0x2C/0x30, retarget
countdown at 0x34, and two x/y scroll-offset pairs at 0x38/0x40. The family name
describes the visible layer behavior; its gameplay identity remains tentative.

State table gUnk_086059DC contains five handlers. Inactive clears shadow
DISPCNT bit 200 (BG1) on entry. Appearance enables the layer with decreasing
hidden-frame frequency over three fifteen-frame ranges, then enters state 2
after frame 44. State 2 shows alternate frames and uses page zero. State 3
alternates scroll pages every frame while leaving BG1 enabled. Disappearance
blinks for ten frames, then clears the layer and enters inactive state.

States 2 and 3 forward different caller-supplied effect sources every ten
overall frames through the object at gUnk_02000580. A null object suppresses
that forwarding. State transitions reset only the state counter and set the
consumable entry flag; overall frames continue. The outer update increments
both counters after dispatch and scrolls only while the state remains nonzero.

Scrolling combines the projected camera vector with camera-block offsets.
The two scroll pages advance on alternate overall frames at different rates.
Pixel phases wrap to eight bits. Tilemap origin uses signed division by eight,
with the y origin forced even. The current speed approaches its target by one
step every sixteen frames. Retarget intervals are **128–255** updates.
Initialization samples current and target speeds in **-4..3**; subsequent
retargeting samples **-1..6**. RNG counter write timing is preserved.

Initialization loads map resource **EA77**, handles its MP signature through
the existing background loaders, and copies eight palette words from resource
**92B3/E39B +194** to RAM gUnk_03005140. As in the original, it assumes these
resources exist. The destructor returns zero and performs no extra cleanup.
Public controls begin appearance, alternating pages and disappearance; the
query returns zero for null. Hidden effect-source/query helpers are already
exact and receive no new credit.

New full extents: 08047718 = 44; 08047770 = 276; 080478A4 = 108;
08047910 = 100; 08047974 = 60; 080479B0 = 68; 080479F4 = 56;
08047A30 = 228. The Update extent ends before the independent four-byte
no-op destructor, so no neighboring bytes are claimed.

## Validation

The combined build printed **`build/boktai3.gba: OK`** in
`build/effects-background-final-combined.log`. **34 post-promotion checks**
passed at complete extents: thirteen new functions, twelve typed existing
functions and nine renamed callers. The existing death-handler draft was
separately rechecked after correcting its spawn prototype and remains 97.0%
with no exact credit. Formatting, function inventory, named RAM literals and
decoded actor-registry checks pass. This is static evidence; no new emulator
coverage or gameplay identity is claimed.

Next contiguous subsection: **08047BB4**, a sixteen-entry temporary animation
pool using a relocated resource and a per-entry lifetime.
