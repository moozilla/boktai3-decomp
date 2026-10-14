# Scene pool state family: second contiguous batch

Exact code advances from 22.381% to 22.502% (+0.121 percentage points):
eight new matches, 2,924 bytes. The total is 6,279 / 11,465 functions and
543,506 / 2,415,354 bytes. Of this batch's bytes, 212 need no register hints
and 2,712 retain the authorized concession after depin.py removed unnecessary
hints. Existing matches were rechecked after shared type changes; those earn
no additional credit.

## State-machine evidence

The ROM function-pointer table at 0860585C contains these twelve callbacks,
in order. The dispatcher at 0803C114 references that table and the entry's
state field at +0x68. Ten callbacks now match; two are retained near matches.
This establishes native state IDs, not the controller's gameplay identity.

| State | Address | Observed role | Status |
|---:|---|---|---|
| 0 | 0803DDA8 | cruise controls and kind-specific state selection | exact, prior batch |
| 1 | 0803DF90 | adaptive speed/lateral tracking and timed action selection | 99.2% draft |
| 2 | 0803E1AC | alternate tracking thresholds and action selection | newly exact |
| 3 | 0803E354 | scheduled speed/lateral controls and metric-based transitions | newly exact |
| 4 | 0803E4D0 | lateral weave with direction reversal and timed exit | newly exact |
| 5 | 0803E5F4 | long arc launch at mode-dependent frame 45/135 | newly exact |
| 6 | 0803E7B4 | short arc launch at kind/mode-dependent frame 10/20/40 | 99.1% draft |
| 7 | 0803E998 | kind-specific single/paired bursts at selected frames | newly exact |
| 8 | 0803EBF4 | timed deceleration, acceleration and 0x300 control sequence | existing exact |
| 9 | 0803EC88 | persistent deceleration control | existing exact |
| 10 | 0803ECCC | timed effects/sound and delayed slot removal | existing exact |
| 11 | 0803ED84 | snapshot position, advance, then stop after 128 shifted units | newly exact |

The dispatcher also establishes the control metrics: entry +0x40 selects a
player-position snapshot (16-byte stride); +0x58 is target x minus actor x,
and +0x5c is target z minus actor z. The legacy header names `motionPhase`
and `direction` are now annotated with those exact relationships; a later
source cleanup can rename them without a new naming investigation.

The dispatcher at 0803C114 selects state 11 on actor flag +0x5ee bit 2,
and state 10 on bit 1. These are static flag consumers; their gameplay events
are not established. The earlier object/lifecycle range shares the same manager:
0803BA84 returns gUnk_02000108; 0803C880 indexes 0x17c pool entries and four
0x8b0 actor slots. Its earlier `LinkScene`/wireless labels remain provisional.

## Corrections and recovered layout

The former `ScenePool_UpdateSteeringPulse` is now `ScenePool_UpdateSpeedPulse`:
its signed pulse at entry +0x67 writes 0x10/0x20, which the exact motion helper
uses to change forward speed. Lateral controls are 0x40/0x80. The three byte
fields +0x65–+0x67 and threshold type were renamed accordingly.

The speed helper uses five signed thresholds from each of three tables at
0860588C, 086058A0 and 086058B4. It enters/leaves its active window, waits
30 or 60–91 ticks, emits signed pulses of magnitude 16, and chooses more
aggressive control flags outside the window. The exact speed scheduler at
0803DAC8 fills setting/delay +0x16c/+0x170 via adjacent 0803DA30.

Shared `ScenePoolVec32` has three signed coordinates plus an unnamed 32-bit
padding word. The original copy operations transfer all 16 bytes; C reads
only the coordinate members. This preserves representation copies without
inventing a fourth coordinate. Native effect creator 08114928 also copies
16-byte position and velocity representations. Actor and entry positions,
saved actor position at +0x8a0, and the manager's four actors are typed directly.
All affected exact consumers still match.

Further actor fields established by these callbacks: forward bias at +0xc0,
emission-descriptor prefix at +0x2b4 (observed first 12 bytes; full extent
unknown), halfword +0x308, byte +0x57a, flags byte +0x5ee and saved position
+0x8a0. The burst state calls 08114928 with a queried speed plus one, twice
for kind 3, and calls `Anim_SetKeyframes` on actor regions +0x660 and +0x7e0 with index 30,
mode 2 and flags 0. Those regions remain opaque; 30 is not claimed as a duration.

## Acceptance and remaining hypotheses

New exact addresses and reviewed extents: 0803DAC8 (236), 0803DC60 (328),
0803E1AC (424), 0803E354 (380), 0803E4D0 (292), 0803E5F4 (448),
0803E998 (604), 0803ED84 (212). Every one passed a resolved-byte full-extent
check. The single batch build is recorded in HANDOFF.md after it completes.

0803DF90 is full-sized but differs in a conditional branch expanded into an
inverse condition plus a jump, shifting nearby branches/alignment. Removing
its exit barriers merges required state-entry sequences; a 45-second permuter
search produced no improved candidate. Do not repeat those trials blindly.

0803E7B4 has only two byte differences: a kind-pointer reload uses r3 rather
than r7. Direct r7 pins cause an extra pointer copy during reload; reserving
other registers changes earlier lifetimes and is not accepted. A bounded
permuter search ended with its AST-node assertion and no saved output. This
campaign made no tool changes. Both best valid drafts are guarded and have
recorded fuzzy scores; neither is exact progress.

Remaining in the current controller: actor setup 0803CEE8, registry/bounds scan
0803D288, manager loop 0803D430, and those two near state drafts. Earlier
0803C114 dispatch/update and collider setup 0803CC90 also remain assembly.
The next adjacent subsection starts at 0803EE58 and contains the arc effects
created by these states, including large update 0803EF4C and allocator 0803F4CC.
Continue with their shared vector/graphics context; do not sweep unrelated ranges.
