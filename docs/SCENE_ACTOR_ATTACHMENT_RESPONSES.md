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
