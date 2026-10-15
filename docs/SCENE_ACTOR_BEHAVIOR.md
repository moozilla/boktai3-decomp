# Scene actor behavior frontier

This pass follows the adjacent actor placement/animation work into **08078078–0807A62C**. The first observed helper cohort ends at **08078EE8**; the following inventory entry occupies5,956 bytes through7A62C and remains under review. Names are provisional behavior descriptions, without runtime coverage claims.

The current frozen set has **eight changed TUs /1,420 reviewed bytes**, including **five newly authored exact bodies /1,132 bytes**. All five retain minimized register concessions, with no explicit r7 binding. Because196 of those bytes previously received false retained-tail credit, the net gain against the earlier inventory is **936 bytes /+0.0388 percentage points**. The other288 reviewed bytes were existing C, now typed and named without new credit. Parent integration owns promotion and one combined ROM build.

## Reviewed boundaries

Two complete independent routines were hidden in `.incbin` tails:

| Start | Reviewed end | Bytes | Evidence |
|---|---|---:|---|
|0807836C|08078430|196|Own PUSH-r4/r5/r6/LR, blink control flow, POP/BX and final alignment after sound routine return/literal pool.|
|08078550|0807864C|252|Own PUSH/save prologue, independent descriptor steering body, POP/BX and alignment after78430 return.|

Both are registered in `symbols/build_extra_functions.csv` and `symbols/progress_extra_functions.csv`. Full-ROM stored Thumb-pointer searches and halfword-decoded B/Bcc/BL target scans found no references; independent complete function structure supports the boundaries. This adds two functions and leaves the code-byte denominator unchanged. The existing sound routine7827C is **240 bytes**, not its earlier436-byte retained span; its complete C checks exact at240. The following196-byte blink routine is now actually matched.78430 ends78550 and covers288 bytes, rather than claiming the252-byte independent successor.

## Proximity and script helpers:78078–7836C

78078 checks players0 through the index held in02000544, skipping absent player pointers. It returns true when the actor/player absolute wrapped signed16 X and Z differences are each at most255. If no player qualifies, it checks the first registered kind4 contact object's position atC with the same test. Y is ignored. The full232-byte routine is exact; depin reduced7→1 hint.

The proximity source uses `ContactPackedVector` and initializes x/z before reading those same members. It never reads the uninitialized y member or the entire storage word. The compiler's storage-unit RMW may preserve unspecified neighboring bits; this is a valid member initialization, not an uninitialized scalar/union read.

78160 invokes the low16 script ID at actor1F4 only when flags2D0 bit80000 is clear, the terrain cell has none of flags43, unsigned ground height2BE minus signed actor Y is at most256, and there is no dynamic cell override. The first terrain lookup prefers that override, otherwise using the map's four-byte base cell records. The12 script values are actor XYZ, words1FC/200/204, -1,0, category98, paletteOffset1E6, parameter2D8 and-1. `ScriptUtilityArguments.count` is initialized as its own bitfield member, and the values pointer is initialized before the call. The full260-byte routine is exact; depin9→2 hints.

78264 invokes the low16 action-script ID1F0 with a NULL argument block when nonzero; its24 old bytes remain exact.7827C dispatches reaction sounds from category98, paletteOffset1E6 and flags2D4 bits6. Its240 old bytes remain exact. The higher-level interpretation of the1E6 selector remains tentative despite sharing the registry paletteOffset storage.

## Drawable blink and steering:7836C–78700

7836C chooses a signed cadence from08605C20: index2 at age≤10, index1 at11..30, index0 above30. At cadence boundaries it toggles drawable flag1, using sequence flags0 for drawable kind0 and tile flags28 otherwise. Age>60 always clears the flag and returns true; younger ages return false. Full196 bytes match; depin31→4 hints.

78430 uses the descriptor's destination1C to derive untruncated signed X/Z differences. Actor flags2D4 bit1000 select a special stopping rule: descriptor flag1 must be absent and the signed interpretation of wrapped squared distance must be≤4000. Otherwise the descriptor's positionC is compared to independent unsigned X/Z tolerances. Outside the stopping rule, nonzero destination deltas set heading59 and produce sine-table acceleration from the fifth speed argument. Components with absolute magnitude>10 are added to velocity64/68; both velocity halfwords are stored even when unchanged. Full288 bytes match; depin41→8 hints.

The steering source explicitly uses unsigned products for wrapped32 multiplication and casts to signed for the observed comparisons. Negative fixed-point scaling expresses wrapped negation in unsigned arithmetic before its signed arithmetic shift, retaining original INT_MIN machine behavior without signed-negation overflow. Initialized speed lifetimes keep natural load-scratch allocation; no handwritten instructions are present.

78550 is a complete private ordinary-C descriptor acceleration draft. It uses wrapped signed16 destination deltas, positional tolerance checks, a byte heading, and adds scaled sine components plus actor halfwords310/314 to velocity. Its full252-byte draft differs only in two addition-order swaps (98.4% raw fuzzy). A bounded15-minute permuter search is running against a reviewed ROM reference. Hidden entries are raw incbins in split output, so the private reference is generated from original ROM halfwords with Thumb code markers and the two literal pools/data padding marked separately; the initial data-only reference was rejected because its score was misleading. No candidate from that invalid setup is credited.

7864C uses wrapped signed16 destination deltas. Outside the descriptor's X/Z tolerances, it compares target/current facing quadrants derived from heading+32. A different quadrant writes requestedDirection3CD=(targetQuadrant*2+3)&7 and returns true. Otherwise it returns false. Full156 bytes match; depin8→4 hints.

786E8 initializes the actor's shadow at320 using its position5C and zero flags. Its24 old bytes remain exact, with a typed actor formal.

## Remaining contiguous work and verification

The adjacent78700–78EE8 helpers configure colliders and process contact/reaction state; their full contracts are still being reviewed. The old108-byte78B48 body remains existing C and is not part of this frozen review count. The large78EE8 inventory span is not yet promoted from private assembly.

Fast full-extent proofs are in the worker's private `build/behavior-{near,script,action-script,sound,blink,accelerate,turn,shadow}-final.json`. No worker complete ROM build was run. The earlier72854 terrain and72F4C contact-evaluator semantic drafts, and6D184/6D438/6D740 route drafts, remain private and uncredited. Their evidence and search history are retained in the earlier subsystem notes and build artifacts.
