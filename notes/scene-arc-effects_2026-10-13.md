# Scene arc slots and projected ground effects (tentative)

This contiguous continuation covers **0803EE58–0803F804**. Gameplay identity
remains provisional; static execution roles establish the names, not coverage.

Exact C rises from **22.502% to 22.529%**, **+0.027 percentage points**
(+644 bytes, four new functions). Session gain from Claude's 22.309% baseline:
**+0.220 points**, 22 new functions, +5,300 bytes. Without register pinning:
18.466%; Register-pinned: 4.063%, included above. Only the new factory needs
hints (depin reduced 20 to 10); the other three additions have none.

| Address | New exact function | Reviewed bytes |
|---|---|---:|
| 0803EE5C | SceneArc_InitializeSlot | 164 |
| 0803F4CC | SceneArc_Create | 248 |
| 0803F650 | GroundEffect_UpdateProjection | 64 |
| 0803F730 | GroundEffect_Initialize | 168 |

## Ownership and layout

The FE2C resource-registry entry owns a 0x9a0-byte task with eight 0x130-byte
slots beginning at +0x20. Each slot contains timing fields, padded 16-byte
start/current/target vectors, a collider at +0x58, animation data at +0x98,
a sprite at +0xb4, animation state at +0xe0 and a 0x40-byte ground effect at
+0xf0. Slot creation is called by ScenePool_StateLaunchArc and the retained
ScenePool_StateLaunchShortArc draft. A missing manager is created, but that
call returns -1; the caller must try again later. Release time is endFrame+8.

Slot initialization loads E74B sprite data, initializes zero start coordinates,
and configures the collider with packed signed 16-bit origin=(0,0,0),
extent=(8,4,8), offset=(0,4,0), plus the original numeric parameters. The packed
bitfield representation reproduces the compiler's word updates; unused padding
is not promoted to an initialized logical coordinate.

The F0DE registry entry owns a separate 0x20-byte task and linked list. Effects
link at the head, carry prev/next pointers and refuse duplicate activation.
Their world-position pointer is projected onto screen coordinates using camera
x at +0x44 and terrain helper 0811AE68. Kind zero projects each update; other
kinds start disabled and are enabled explicitly by the arc factory. Calling
GroundEffect_EnableProjection immediately projects once. A graphical shadow
role is plausible but remains tentative. GroundEffect_Destroy also has the
separate 081D0B8C caller, so this utility is not exclusive to the arc pool.

Existing exact helpers were named and the slot/list consumers were converted
to shared types. GroundEffect_EnableProjection/DisableProjection now assign
symbolic function pointers instead of the former literal Thumb addresses.
All affected existing bodies passed exact checks; these edits earn no new credit.

## Retained update draft

SceneArc_UpdateSlot (0803EF4C, reviewed 1,112 bytes) is guarded NONMATCHING,
**67.3% fuzzy**, and earns no exact credit. It compiles to 1,120 bytes. Its C
records launch interpolation, sine lift, terrain clamping, post-flight drift,
landing collision/effect activation, timed release and inline animation advance.
Player flags at 020004F0[0]+0x8f4 bit 2 suppress landing/release handling.
The post-flight direction builder consumes target.y without a >>12 shift;
that surprising original operation is preserved and commented.

Allocation experiments adding a separate r9 sprite alias and a pinned
projection inline helper worsened the score (53.8–66.6%) and inflated the body.
The retained first candidate is simpler C without pins. The remaining stack
layout, duplicated sprite lifetime and projection reloads need a new source
hypothesis; do not repeat blanket register pinning. No bounded permuter was
used because control-flow and instruction ordering still differ substantially.

## Validation and next work

Every exact function was checked through its full reviewed extent. One combined
ROM build gates this batch (`build/scene-arc-combined.log`); no draft receives
exact credit. The next contiguous block begins at **0803F804**: bitmask/count
helpers and six-bit packed game-state values, followed by unmatched consumers
0803F95C, 0803FA5C, 0803FC8C, 0803FCC4 and 0803FDBC. Claude already registered and matched the former hidden 0803F8F4 span;
reuse its C and do not count it as a new match.

Continue the current 25% / 80%-remaining goal. Latest account observation:
**93% remaining**. Main-only progress Actions and batched integration remain
in force. The original checkout's uncommitted Claude edit remains untouched.
