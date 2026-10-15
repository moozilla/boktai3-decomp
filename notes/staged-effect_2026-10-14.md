# Contiguous staged effect subsection (Sol, 2026-10-14)

New exact code: **0.0750 percentage points** (1,812 bytes, ten functions),
relative to main 36ec8e7c. Of those, 1,336 bytes require no hints and 476 bytes
retain minimized register-pinning concessions. Full reviewed extents pass the
fast checker; the integrator owns combined ROM validation. No emulator claims.

The entire **0804D778–0804DF40** subsection is now exact (1,992 bytes,
including pre-existing matches). It is a five-sprite scripted effect; the
specific visual/gameplay identity is tentative, so names use `StagedEffect`.

## Recovered contract

`include/staged_effect.h` describes the observed 0x190-byte task allocation.
The effect owns five 0x2C-byte display sprites at 0x74. Each sprite has flags at
0, render mode at 7, X/Y scale bytes at 8/9, priority at 0x10 and XYZ halfwords
at 0x1C/0x1E/0x20. The parent task has script-presence/state/palette/scale
halfwords at 0x4C/0x4E/0x50/0x52, release frame at 0x54, travel/initial travel
at 0x58/0x5A, spread/initial spread at 0x5C/0x5E, shake X/Z at 0x60/0x62,
frame at 0x64, pending script resource ID at 0x68 and packed center XYZ at 0x6C.

Graphics storage begins at 0x150, its palette pointer is 0x15C, the separately
selected active palette is 0x16C, and sixteen blended colors start at 0x170.
Palette interpolation uses RGB555 channel masks directly and divides weighted
channel sums by 64; endpoints select the source palette without copying.
Palette source indices are relative to the global graphics pointer + 0x5060.

Initialization (`DD48`) reads script keyword `0x4F` for an optional script
context and `0x70` for center XYZ (otherwise GameState+0x30/32/34). Config
keyword `0x68` defaults travel to 1024; `0x6D` defaults spread to 512. Keyword
`0x65` supplies a pending script resource ID and release frame. Five sprites share graphics
resource 0x2499. The palette starts from graphics +0x51A0. Callback dispatch
uses the ROM table at 08605A58 and accepts the task pointer. Destruction returns
zero after removing five sprites and its optional script context.

`D874` and `D890` are identical: both OR flag value 1 into sprites1–4. Their
names deliberately avoid guessing distinct hide/show behavior. State callbacks
also use the low flag bit for center-sprite reveal and script completion.

## New exact functions

- D620 Countdown_Tick, 184 bytes, no hints.
- D778 StagedEffect_BlendPalette, 252 bytes, no hints.
- D910 StagedEffect_WaitForScript, 68 bytes, no hints.
- D980 StagedEffect_ShakeAndReveal, 140 bytes, minimized hints.
- DA0C StagedEffect_ContractBurst, 188 bytes, no hints.
- DAC8 StagedEffect_BlendAndShake, 152 bytes, minimized hints.
- DB60 StagedEffect_WaitBetweenShakes, 32 bytes, minimized hint.
- DB80 StagedEffect_PlayShakeSequence, 152 bytes, minimized hints.
- DC40 StagedEffect_Tick, 208 bytes, no hints.
- DD48 StagedEffect_Initialize, 436 bytes, no hints.

The separate countdown task uses GameState+0x618 as remaining count and +0x61C
as its UI companion value. Existing D6DC sets the task interval from script
keyword0x73 (default30), initializes frame0, and clamps a nonpositive remaining
count to1. D620 advances only while player+0x1C is1. At expiration it calls the
player reset API, writes transition0x24 at+0x457, clears phase+0x458/frame+0x45A,
and kills itself, except when transition already equals0x18. Global player
pointer reloads between writes are preserved.

No gaps/drafts remain in the completed effect subsection. Continue at 0804E180
(the surrounding DF4C/E030 descriptor helpers were already exact).
