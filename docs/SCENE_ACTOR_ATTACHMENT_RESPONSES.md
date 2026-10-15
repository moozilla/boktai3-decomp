# Actor attachment selection and responses

Reviewed subsection: **0807C11C–0807E334**, 22 entries / 8,728 bytes, including
previously hidden selector entries C468 and C53C. This follows the actor animation
and placement helpers and ends after the complete DB34 response body.

## Exact prefix and neighboring helpers

The current worker has checked 17 entries / 1,936 bytes exactly. Of those,
**1,472 bytes are new exact C**: 1,280 ordinary C and 192 register-pinned bytes.
The other 464 bytes were already exact (C374, C468, C53C, C644, D1E8), and receive no
additional progress credit. This is approximately **+0.0610 percentage points**
against the current code denominator; the integrator derives the authoritative
whole-repo percentage after merging metadata. The adjacent core query
0822B694 is also reviewed as existing exact and earns no additional credit.

| Address | Size | Contract | Classification |
|---|---:|---|---|
| 0807C11C | 600 | Category/event/variant sound selection | New ordinary |
| 0807C374 | 32 | Call sound selector only when gate byte is zero | Existing ordinary |
| 0807C394 | 68 | Category 4 Y/Z offsets | New ordinary |
| 0807C3D8 | 76 | Category 1 Y/Z offsets | New ordinary |
| 0807C424 | 68 | Categories 5–8 Y/Z offsets | New ordinary |
| 0807C468 | 76 | Alternate common offsets; no observed call yet | Existing ordinary |
| 0807C4B4 | 64 | Category 3 Y/Z offsets | New ordinary |
| 0807C4F4 | 72 | Category 25 Y/Z offsets | New ordinary |
| 0807C53C | 68 | Alternate category 21 offsets; no observed call yet | Existing ordinary |
| 0807C580 | 68 | Category 21 Y/Z offsets | New ordinary |
| 0807C5C4 | 64 | Category 10 Y/Z offsets | New ordinary |
| 0807C604 | 64 | Category 23 Y/Z offsets | New ordinary |
| 0807C644 | 232 | Dispatch category-specific offset selector | Existing ordinary |
| 0807CA00 | 68 | Test halfword-difference X/Z window | New ordinary |
| 0807CD80 | 68 | Stable ascending signed-key candidate sort | New ordinary |
| 0807D1E8 | 56 | Tagged-position lookup with grid fallback | Existing ordinary |
| 0807D220 | 192 | Retarget to nearest non-null player | New register-pinned |

`scene_actor_attachment.h` records the typed prefix APIs. C644 writes signed
32-bit Y/Z offsets: C72C's stack values feed the Y/Z arguments of attachment
constructors 65DBC/65FD0, and the Z argument of 66278/66AB4. These are not resource
IDs. No selector writes its outputs for an unsupported kind; C644 also leaves
them untouched for an unsupported category. Thus callers using uninitialized
local outputs require a supported category/kind, rather than invented defaults.

### Sound selection

Arguments to C11C are narrowed to bytes. Events 0 and 3 share one sound; event 1
and event 2 choose the other sounds shown below. Category 10 also shares its
first sound with event 1. Category 3 accepts variant bytes 0, 1, 3, or 5 only.
Unsupported category/event/variant combinations do nothing. C374's first byte
is an additional zero-valued gate. Its four arguments have word ABI formals and
are explicitly narrowed to local bytes. Narrowing all four at the declaration
boundary added unwanted caller instructions once the caller's hints obscured
value ranges; word forwarding preserves the observed call sequence. Declaring
the gate byte local first also preserves C374's ordinary exact prologue.

| Category | Events 0/3 | Event 1 | Event 2 |
|---|---:|---:|---:|
| 4 | 240 | 239 | 241 |
| 1 | 259 | 258 | 260 |
| 3 | 390 | 389 | 391 |
| 21 | 932 | 931 | 315 |
| 10 | 625 | 625 | 626 |
| 5/6/7/8/23 | 314 | 313 | 315 |
| 25 | 456 | 455 | 457 |

Keeping the two identical category-10 sound-625 calls in distinct switch bodies
reproduces the original switch lowering and branch layout in ordinary C; grouping
all three case labels changed branch choices elsewhere. No register hints needed.

### Position window and candidate sort

CA00 truncates each X/Z difference to a signed halfword, takes its absolute value,
and compares it to an unsigned word bound, accepting equality. Y is ignored.

CD80 sorts 8-byte `{s32 key, u32 value}` records with stable adjacent swaps,
performing count-minus-one complete passes. Only strictly greater signed keys
swap. Count is a nonnegative element count within valid array storage. Its
explicit decrementing inner loop matches ordinary C; there are no register hints.

### Nearest-player retarget

D220 scans the configured player count at 02000544 across the player-pointer
array at 02000580, skipping null entries. The count must describe valid initialized
array storage, as in the original routine; no invented clamp is added. Each XYZ
difference is stored as a halfword. Only signed X/Z squares affect distance;
their sum wraps in 32 bits before the signed best-distance comparison, preserving
the extreme-coordinate behavior. Ties retain the earlier player. The selected
source address is player+24. If no player wins, the old target stays unchanged.

On replacement, actor flags2D4 bit1000 releases the owned contact-list node at
45C through 72E08, clears the bit and pointer, and then writes source DC. The
ContactPosition padding halfword is never read as a scalar. Standard depin reduced
14 hints to 5; the remaining hints preserve pointer lifetimes and clear-store
ordering. There is no explicit r7 binding or handwritten instruction body.

## Remaining bodies

C72C (724), CA44 (828), CDC4 (1060), D2E0 (2132), and DB34 (2048) remain assembly;
none receives new C credit. C72C's semantic private draft covers frame127 release,
kind-specific attachment replacement, category offset selection, optional sounds,
and the kind1 unavailability check. The last 65C50 return is passed directly as
66C8C's slot argument even though no intervening move appears in assembly.
The supported-category caller precondition still needs its caller evidence audit.

CA44 places a route-related target on terrain and attempts contact/position
resolution, then selects route or response control. CDC4 builds/sorts position
candidates before another search. D2E0 and DB34 are large response/state bodies;
subsystem names beyond their observed effects remain tentative.

## Tagged-position query ABI and remaining preconditions

0822B694 reads an entry in the active 16-byte-stride table at 030025FC, writes
X/zero-Y/Z to the output, and returns its 16-bit tag. Its ABI return is a **word
containing a zero-extended 16-bit value**: declaring u16 at D1E8 added a pair of
caller narrowing shifts absent from the ROM. The corrected core returns u32 and
still matches all 52 bytes. The index is also a word formal, narrowed to a byte
before sign extension to the stride; D1E8 explicitly passes a signed-byte index.
The core's left shift now uses unsigned arithmetic before the signed right shift,
removing the old signed-left-shift overflow while preserving the exact code.

D1E8 writes the tag to +18 and invokes CDC4 with the same input/output vector at
+0C only for tag B546. This is a tagged-position fallback wrapper, not a script
result wrapper. The table index must refer to valid table storage; no new bounds
check is invented. If the table is absent, the core returns zero without writing
the vector. Both core and wrapper remain ordinary exact C.

C72C remains held despite its close candidate: the selector leaves automatic
local offsets untouched for unsupported categories. An inventory found 81 direct
ROM calls, with 41 literal frame127 release calls (which never read those offsets)
and 40 other contexts, mostly forwarding actor category98. The reviewed 78EE8
activation branch excludes categories2/25 but does **not** establish the supported
selector set; 78BB4's three observed dispatcher callers likewise provide no local
category restriction. This is not evidence for unrestricted defined scalar reads.
A broader constructor/configuration proof remains outstanding; no default offsets
or additional exact credit are claimed. The bounded permuter runs on the preserved
conditional candidate while that evidence gap is retained explicitly.

## Reviewed private route/contact drafts

CA44 first selects animation entry0, writes actor destination5D4 from route point
450 (X/Z), and computes its terrain Y. Terrain index uses signed coordinates >>8,
map bounds, and row offsets24; out-of-bounds coordinates select cell0. Height is
low nibble<<8, with slope kinds1/2 subtracting low Z/X respectively.

It then tests contact node45C's object position+0C against actor position5C using
query context5D0: F204 must succeed, then F7A8 must return nonzero. Result2 keeps
velocity; other accepted results clear velocity64. Failure clears flags2D4 bit200.
On first failure it repeats the queries for a temporary route-point vector,
computing terrain height again. The fallback resumes callback228 with actor
state0/phase0/behavior2 and entry1, regardless of the second query outcome.

First-target success instead requests attachment kind0/frame1/category98, selects
actor state3/phase1/behavior0, and installs FDC0 at21C. The embedded response at460
starts state0/phase0/behavior7, timer46C=30, and saved callback470=actor228. Flags2CC
bit10 and flags2D4 bit1000 are set; destination5D4's trailing halfword is cleared.
Both paths clear field2C8, flags114 bit1, and field104. The field2C8 clear is not a
clear of the similarly nearby route cursor448. Callback interpretations remain
tentative. FDC0's first operand is an actor pointer and its second operand is a
signed threshold compared to timer46C; its reviewed epilogue has no explicit
scalar return.

CDC4 first accepts the supplied candidate if its terrain halfword attributes lack
maskC3 and F204 accepts it relative to source position+0C. Otherwise it constructs
four candidates in order: cell+1, cell+rowStride, cell-1, cell-rowStride. Each center
uses signed Mod/Div by the positive map stride, X/Z center128 within the cell, and
Y=(heightLowNibble<<8). This neighbor Y calculation **does not** apply the slope
subtraction used by CA44. Each key is a halfword-truncated X/Z squared distance
from source; Y delta is also written but does not affect the key. The sum wraps
before signed comparison. CD80 sorts these keys stably, so equal-distance ties
retain right/down/left/up order. The first sorted candidate with clear C3 terrain
bits and a successful F204 query is copied to output. Failure writes X/Z=30000
and Y=0, preserving the output padding halfword.

All named XYZ components in the private CDC4 draft are initialized. Eight-byte
aggregate transfers may carry unspecified padding; no padding scalar is read.
The map must have a positive nonzero stride and valid backing storage for the
four queried indices. Neighbor index arithmetic is kept in unsigned integer
address formation before pointer conversion, preserving target wrapping without
an intermediate huge/out-of-bounds C pointer addition for negative indices.
These drafts remain private and uncredited; their matching and caller/storage
preconditions still require completion before promotion.

## Assembly-only mode and contact-release responses

D2E0 has five incoming operands: actor, signed mode, position pointer, Y adjustment,
and a signed terminal/nonterminal selector. Only modes0/1 use the supplied position;
mode2 applies a health/fall response. These interpretations remain tentative.

Before dispatch it clears actor2B8. If flags2D4 mask0x2000 is set, it emits three
08231500 effects using actor47C configuration and origin484, with origin Y+128,
random jitter X/Z in[-7,8], Y in[-16,15], packed vector(5,10,5), and two trailing
40 arguments. This advances the shared random-table cursor nine times. It then
clears actorAA/2B8 and flags2D4 mask0x2000. Invalid modes return after this prefix.

Modes0/1 select response21/callback5FC unless substate2B5 is already21, set flags2D0
masks0x20000 and0x4, reset several contact/timer flags, and copy the supplied
position to actor5C with the requested Y adjustment. The copy is mirrored to the
appropriate sequence/tile drawable at1C/40, then flags2D6 mask0x2000 is set.

Mode2 disables shadow320 and hides the drawable. A positive fifth operand applies
the terminal path: old health goes to collider134.cost(+3E) and actor2E4, health
becomes zero, owned attachments0/1/3 are released through frame127 calls, E7B0 is
called with4, and response17/callback234 is selected. Several existing multiplayer
membership and scene gates control reporting through1781B4 and damage-number
display. In nonmultiplayer mode, flags2D0 mask0x800 suppresses the6FA74 removal
record. Categories1/4 emit six 6A160 bursts with packed vector(80,60,60), random
mode1..4, and headings32/64/94/128/196/240; a subsequent live category check gates
song229. The nonpositive fifth-operand path instead computes health/2 toward zero,
stores that as collider cost/actor2E4, subtracts it with halfword truncation and a
zero floor, and performs the gated reporting/display path without the terminal
state transition. No C body for D2E0 is promoted yet.

DB34 is a contact-release/route-resolution response, rather than a generic death
routine. It first releases node45C if flags2D4 mask0x1000 was set, clears that bit
and pointer, sets2C2=64, selects animation0, and chooses response14/FDC0 with actor
state3/phase1/behavior0. Its subsequent branch is controlled by flags2D4 mask0x80.

In that branch it refreshes destination5D4 from route450 and terrain height. The
source cell comes from the targetSourceDC object's X/Z in single-player mode; in
multiplayer mode, its byte8 identifies a player slot0..3, and the cell comes from
that player's object40 halfword6. Missing/invalid multiplayer source indices take
the fallback callback path. The source cell is compared to actor2F4, with a player
status0x20000 exception. A differing cell attempts F204/F7A8 at that cell's center;
accepted results install response behavior12/timer60/callback41C. Query failure
clears flags2D4 mask0x200 and calls actor5F8 through the r1 trampoline.

The same-cell route may select substate26/callback41C and actor behavior12, or the
ordinary route callback228/behavior5, preserving the observed reread of mask0x80
across calls. Unless flags2D4 mask0x400 or flags414 mask0x4 blocks it, the routine
pushes a temporary direction response: saved callback21C goes to460, state/phase/
behavior go to464/465/466, substate2B5 to467, and facing59 to5D9. It chooses a random
cardinal direction(0/64/128/192), retrying equality with the old direction at most
four times, and writes it to5D8. It selects callback5F0/substate27 with actor3/1/0,
sets5DA=6, then calls E334 and sets2C2=32. These offsets directly connect this
subsection to the following worker's E334 controller.

With the initial mask0x80 clear, DB34 marks destination5D4's fourth halfword1 and
queries it directly. Failure calls actor5F8; success starts response behavior7,
timer60, callback228. The usual result2 exception preserves velocity. Both response
paths retain the actor420/flags reset details in assembly; no exact C credit is
claimed for DB34. The fourth vector halfword is sometimes explicitly written as
phase metadata here, so its role should not be generalized as meaningless across
all vector views; the reviewed F204/F7A8 chain uses its coordinate components and
whole aggregate copies, without a scalar read of that fourth halfword.
