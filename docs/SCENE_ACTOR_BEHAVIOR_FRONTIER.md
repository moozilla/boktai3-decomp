# Scene actor behavior frontier

This pass follows the adjacent actor placement/animation work into **08078078–0807A62C**. The first observed helper cohort ends at **08078EE8**; the following inventory entry occupies5,956 bytes through7A62C and remains under review. Names are provisional behavior descriptions, without runtime coverage claims.

The current frozen set has **14 changed TUs /3,128 reviewed bytes**, including **ten newly authored exact bodies /2,732 bytes**. The descriptor acceleration252-byte body is ordinary C; the other2,480 newly matched bytes retain minimized register concessions, with no explicit r7 binding. Because196 of those bytes previously received false retained-tail credit, the net gain against the earlier inventory is **2,536 bytes /+0.1050 percentage points**. The other396 reviewed bytes were existing C, now typed and named without new credit. Parent integration owns promotion and one combined ROM build.

## Reviewed boundaries

Two complete independent routines were hidden in `.incbin` tails:

| Start | Reviewed end | Bytes | Evidence |
|---|---|---:|---|
|0807836C|08078430|196|Own PUSH-r4/r5/r6/LR, blink control flow, POP/BX and final alignment after sound routine return/literal pool.|
|08078550|0807864C|252|Own PUSH/save prologue, independent descriptor steering body, POP/BX and alignment after78430 return.|

Both are registered in the inventory (`symbols/progress_functions.csv`). Full-ROM stored Thumb-pointer searches and halfword-decoded B/Bcc/BL target scans found no references; independent complete function structure supports the boundaries. This adds two functions and leaves the code-byte denominator unchanged. The existing sound routine7827C is **240 bytes**, not its earlier436-byte retained span; its complete C checks exact at240. The following196-byte blink routine is now actually matched.78430 ends78550 and covers288 bytes, rather than claiming the252-byte independent successor.

## Proximity and script helpers:78078–7836C

78078 checks players0 through the index held in02000544, skipping absent player pointers. It returns true when the actor/player absolute wrapped signed16 X and Z differences are each at most255. If no player qualifies, it checks the first registered kind4 contact object's position atC with the same test. Y is ignored. The full232-byte routine is exact; depin reduced7→1 hint.

The proximity source uses `ContactPackedVector` and initializes x/z before reading those same members. It never reads the uninitialized y member or the entire storage word. The compiler's storage-unit RMW may preserve unspecified neighboring bits; this is a valid member initialization, not an uninitialized scalar/union read.

78160 invokes the low16 script ID at actor1F4 only when flags2D0 bit80000 is clear, the terrain cell has none of flags43, unsigned ground height2BE minus signed actor Y is at most256, and there is no dynamic cell override. The first terrain lookup prefers that override, otherwise using the map's four-byte base cell records. The12 script values are actor XYZ, words1FC/200/204, -1,0, category98, paletteOffset1E6, parameter2D8 and-1. `ScriptUtilityArguments.count` is initialized as its own bitfield member, and the values pointer is initialized before the call. The full260-byte routine is exact; depin9→2 hints.

78264 invokes the low16 action-script ID1F0 with a NULL argument block when nonzero; its24 old bytes remain exact.7827C dispatches reaction sounds from category98, paletteOffset1E6 and flags2D4 bits6. Its240 old bytes remain exact. The higher-level interpretation of the1E6 selector remains tentative despite sharing the registry paletteOffset storage.

## Drawable blink and steering:7836C–78700

7836C chooses a signed cadence from08605C20: index2 at age≤10, index1 at11..30, index0 above30. At cadence boundaries it toggles drawable flag1, using sequence flags0 for drawable kind0 and tile flags28 otherwise. Age>60 always clears the flag and returns true; younger ages return false. Full196 bytes match; depin31→4 hints.

78430 uses the descriptor's destination1C to derive untruncated signed X/Z differences. Actor flags2D4 bit1000 select a special stopping rule: descriptor flag1 must be absent and the signed interpretation of wrapped squared distance must be≤4000. Otherwise the descriptor's positionC is compared to independent unsigned X/Z tolerances. Outside the stopping rule, nonzero destination deltas set heading59 and produce sine-table acceleration from the fifth speed argument. Components with absolute magnitude>10 are added to velocity64/68; both velocity halfwords are stored even when unchanged. Full288 bytes match; depin41→8 hints.

The steering source explicitly uses unsigned products for wrapped32 multiplication and casts to signed for the observed comparisons. Negative fixed-point scaling expresses wrapped negation in unsigned arithmetic before its signed arithmetic shift, retaining original INT_MIN machine behavior without signed-negation overflow. Initialized speed lifetimes keep natural load-scratch allocation; no handwritten instructions are present.

78550 now matches its full252 bytes in ordinary C, with zero register hints. It uses wrapped signed16 destination deltas, positional tolerance checks, a byte heading, and adds scaled sine components plus actor halfwords310/314 to velocity. The explicit grouping `(u16)oldVelocity + (u16)(bias + step)` preserves the required addition order while the final halfword store makes extra normalization unnecessary. The compiler naturally chooses r7 for the old-velocity load scratch; no binding is used.

The earlier raw98.4% draft differed only in two addition-order swaps. A bounded15-minute permuter search improved its weighted score410→355; a reviewed355 snapshot was then frozen for a four-hour single-worker search. Manual grouping/cast exploration found the exact ordinary-C form while that longer search ran, so its own process group was stopped safely. Hidden entries are raw incbins in split output, so the private reference was generated from original ROM halfwords with Thumb code markers and the two literal pools/data padding marked separately. The initial data-only reference was rejected because its score was misleading; no result from that invalid setup is credited.

7864C uses wrapped signed16 destination deltas. Outside the descriptor's X/Z tolerances, it compares target/current facing quadrants derived from heading+32. A different quadrant writes requestedDirection3CD=(targetQuadrant*2+3)&7 and returns true. Otherwise it returns false. Full156 bytes match; depin8→4 hints.

786E8 initializes the actor's shadow at320 using its position5C and zero flags. Its24 old bytes remain exact, with a typed actor formal.

## Remaining contiguous work and verification

78700 now matches all340 bytes (depin47→8 hints). It disables the primary collider134 for flags2C0 bit800, special categories12/13/23, kinds21/22, flags2D4 bit8000, or absent flags2D6 bit4000. The4000 flag is consumed when entering the active path. Secondary collider188 is refreshed at the actor position when flags2D4 masked110 are present, or proximity78078 succeeds without bits6; primary placement follows separately. These are distinct54-byte collider records.

78854 now matches all408 bytes (depin35→5 hints). It rejects unavailable actors, locked primary contact parameter178, disabled primary collider flag4, or source collider missing flag1. A locked actor with nonzero mode2E6 instead clears destination remaining3E and mode43. The active path delegates to0821FF8C, copies parameter44 and mode43, adjusts power40 from category or source reactionFlags38 bit40000 minus actor defense9F, and calls0812F520. It copies source kindA/parameter34/reactionFlags38 into actor2E0/2D8/2DC. Game flags868 bit800 or source reaction bit40 clear remaining/power, force mode2E6=1, and record destination remaining into actor2E4. It then calls08159860 and sets actor flags2CC bit2000. Callee08159860 ignores its incoming second pointer, but the observed three-argument caller ABI is retained.

78B48's108 existing bytes now use the shared view. Positive counter304 invokes789EC, updates flags2D6 bits80/100 and enables gate flag10; otherwise gate flag2 permits clearing flag10.789EC now matches its full348-byte extent (depin11→5 hints): clear actor flags2CC bit80/flags2D4 bit1, call7C72C with category, and update capped per-player counters884/886/888/88A (slot0 additionally updates530), unless engine/actor exclusion flags apply. The counters compare signed16 values against9998 before capping at9999. Explicit stores and returns in each player branch preserve the separate comparisons and increments with a shared cap path. This removes three redundant result barriers, shortening an engine-flag branch and restoring literal-pool alignment; the prior352-byte near match differed only in that long branch and padding.

78DEC now matches all252 bytes (depin28→4 hints). Unavailable actors lose flags114 bit1. Without that pending flag it clears savedControl104/allocation108. With the flag, it clears it, copies savedControl104/savedKind110 into control21C/kind2B5, initializes prepared2B2/substate2B0/field2C8, repeats the observed flag clear, clears savedControl104, and releases allocation108 using argument10C through0824923C when present. The post-call clear uses a cached allocation-field pointer, allowing the actor pointer's lifetime to end before the call.

The sole remaining prefix gap is78BB4, totaling568 bytes.78BB4 connects the actor's view beginning50 to the earlier attachment subsystem: follow identity0 is actor ID50, follow direction9 is heading59, and follow anchorC is position5C. Its third/fourth/fifth byte arguments select resource kind, frame and category; frame127 releases a matching resource without creating another. The category-dispatch7C644 output contract needs a caller-precondition audit before any matching draft may read its output locals: unsupported categories return without writing them. Its only direct callers are three attachment command cases in the0812E7xx dispatcher. All pass kind0, frame0/1/2, and the actor category98 as the fifth argument. This does not establish the supported-category precondition: categories1,3,4,5..8,10,21,23,25 write outputs, while other categories return without writing them. The private78BB4 draft initializes its output locals to zero and therefore remains uncredited pending stronger evidence.

## Whole actor update:78EE8–7A62C

The complete5,956-byte entry has one prologue and final epilogue, without evidence of independent hidden entries. A full private semantic draft now covers all branches and compiles to5,856 bytes at39.51% fuzzy; this is not exact progress. Unknown field accessors are confined to the private draft until the phase views are reconciled.

The update first handles attachment-valueAA and timer428/5C8 decay for available actors, then controls the placement/collision flag408 from actor flags, primary contact timer178, vertical state2EA/2EC/2F0 and availability. It chooses effect selector5CC from game progress876 and actor flags2CC bit80000. Palette handling has separate flash1E8 and blink1EA timers, base/alternate banks and global/player-dependent transition71E00 calls. In particular, the active flash path skips the global transition decision for that frame.

Reaction flag1000 resets route motion, saves actor X/Z into310/314 and318/31C, clamps attachment value againstA6, and selects defeated control234, alternate control230, or default5DC. The latter path releases the prior allocation using61C. Actor primary contact timer178 controls drawable/shadow blinking. The following priority chain handles flags2D4 bits2000,4,2,8000,10,8, including interaction45C cancellation, route-state resets at420, control locking, cooldown2C2 and gate flags. These repeated blocks remain distinct semantic operations even where the private compiler output shares them.

Timer30C emits particles only every fourth remaining frame while actor Y is nonnegative. Four random samples determine the vertical offset, signed direction perturbation, width and lifetime. Particle packet x/y/z are initialized separately; the caller does not read an uninitialized whole storage word. The later route activation path installs control7FDC0/kind14, builds a target from the route450 coordinates, resolves cell height and slope, calls21F204 then21F7A8, and conditionally clears velocity or releases allocation224. Interaction618 cancellation is separate. Finally, the gate object's position supplies facingE9 and wrapped squared distanceF0, and heading59 is copied into1CA/176.

The complete source is preserved privately at `wip/sub_08078EE8.c`, with full-extent proof `build/behavior-update-first.json`. The adjacent789EC counter helper is now exact; the private large draft remains uncredited pending further contract and register refinement.

Fast full-extent proofs are in the worker's private `build/behavior-{near,script,action-script,sound,blink,accelerate,addaccel,turn,shadow,colliders,reaction,defeat,defeated,activate}-final.json`. No worker complete ROM build was run. The earlier72854 terrain and72F4C contact-evaluator semantic drafts, and6D184/6D438/6D740 route drafts, remain private and uncredited. Their evidence and search history are retained in the earlier subsystem notes and build artifacts.
