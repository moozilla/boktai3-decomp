# Actor behavior and collision setup (0807462C–08076034)

This contiguous cohort continues the actor profile/registry review ending at
`7462C`. The natural behavior boundary is `76020`, after `75FC8` crosses the
initial `76000` assignment boundary. The adjacent 20-byte `76020` setter is
included as API glue; profile-driven animation selection starts at `76034`
and belongs to the adjacent worker. The cohort contains 15 entries and
6,664 reviewed bytes. No runtime coverage is inferred from static review.

## Receiver registration and object/context glue

`7462C` skips registration when scene flags `030053F4 & 4800` are set. Otherwise
categories `{4,5,7,6,8,21,25,23,18}` register the receiver at actor+C with the
actor's ID and receiver kind3, then set `flags2C0 & 100`. The category sets and
numeric receiver kind describe the observed contract; they do not establish
specific game-character identities.

`76020(actor,context,kind,object)` writes context3D0, object0, and the low byte
of kind to drawableKind8. It does not write drawable4. This was prior exact C;
its typed review receives no additional matching credit.

## Collision property views

The first collider is embedded at actor+134, the second at +188; their spacing
matches the shared `ContactCollider` size54. Names such as Body/Attack are
provisional interpretations of those two setup groups and their consumers.
All six helpers initialize using the low16 actor ID, argument3=0, argument4=16,
and caller-supplied size/offset pointers. They then set position to actor+5C
and attach the supplied callback and owner through the generic collider API.

The actual engine setters establish the property destinations:

| Actor source | Width | Collider destination | Meaning retained |
|---|---:|---|---|
| 9E | byte | halfword3C | field3C, not the separate cost field3E |
| A2 | halfword | word38 | reactionFlags |
| A4 | halfword | word34 | field34, otherwise unconfirmed |
| B4 + index×2 | halfword | halfword3C | indexed field3C |
| BC + index×2 | halfword | halfword40 | indexed field40 |
| C4 + index×2 | halfword | halfword44 | indexed field44 |
| CC + index×4 | word | word38 | indexed reactionFlags |

`scene_actor_collision.h` records the four-slot layout suggested by the array
spacing. The indexed read helpers use separate GNU trailing-storage views,
so their C expressions do not introduce a four-entry array bound. The selected
entries must lie in valid, initialized storage in the actual actor object.
The setup functions retain the caller's full word index and add no clamp.

An initial naming draft confused actor9E with the collider cost. Source audit
corrected it: `0821FF7C` writes its inputs to 3C,38,34, while `ContactCollider`
places the separate cost halfword at3E. The renamed helpers and all affected
bodies were rechecked against the ROM; this naming correction adds no credit.

## Exact setup contracts

| Address | Bytes | Behavior | Credit |
|---|---:|---|---|
| 0807462C | 108 | Register selected category receivers | New, pinned |
| 08074698 | 288 | Advance route point, run optional script, select behavior | New, pinned |
| 08074A98 | 348 | Update terrain/player height and synchronize drawable position | New, pinned |
| 08074BF4 | 112 | Embedded collider134, flags5109, direct properties, refresh | New, ordinary |
| 08074C64 | 112 | Same, caller flags narrowed to16 bits | New, ordinary |
| 08074CD4 | 96 | Caller collider, narrowed caller flags, direct properties, refresh | New, ordinary |
| 08074D34 | 140 | Embedded collider188, flags2401, indexed properties, field34=0 | New, ordinary |
| 08074DC0 | 120 | Caller collider, flags2001, indexed properties, field34=0 | New, ordinary |
| 08074E38 | 120 | Same, caller-selected field34 | Prior exact, ordinary |
| 08075508 | 128 | Project a forward probe and snapshot prior position | New, pinned |
| 08075FC8 | 88 | Clear action field, test eligibility, request available action | New, pinned |
| 08076020 | 20 | Set object/context/type | Prior exact, pinned |

The last three helpers retain their observed lack of `Collider_Refresh`.
The current frozen subset adds **+0.0638 percentage points** (1,540 bytes):
580 ordinary and960 register-pinned. Twelve of fifteen entries are exact,
covering1,680 reviewed bytes. Prior exact E38 and the setter contribute
140 reviewed bytes without new credit. E38 also corrects the old raw formal
argument widths to the actual collision-engine setters, retaining its match. Each body passed a full-size fast check;
the parent performs promotion and one complete combined ROM build.

Receiver registration reduced5→2 hints, the old setter3→2, and the first
collider initializer3→0. Small ordinary inline property readers reproduced
the original address/read ordering where register barriers caused worse global
allocation. The indexed embedded initializer reduced2→0 hints. No handwritten
instruction bodies or explicit r7 bindings are used.

## Route-point transition

`74698` optionally calls the actor script selected by halfword1EC, passing three
words: the embedded cursor index, actor ID, and category. The embedded cursor
at448 supplies that script index independently of the cursor argument. The
shared script argument convention initializes count and values while leaving
the unused halfword unspecified; the source does not read that halfword.

`0821E618` advances the supplied cursor, wrapping its index by definition byte0,
clearing cursor byte3, and calculating its point from the map base plus10,
definition halfword2, and index×8. Points contain X/Z at0/2 and attributes at6,
which explains the earlier `70070` two-halfword target read through actor450.
`74698` uses the high attribute nibble to index table08610814 and narrows the
word return of proximity helper0821E8C8 to a byte. Proximity selects numeric
behavior2 and setting2C2=64; otherwise the table selects behavior1 or0. Each
branch clears state/phase/counter and marks field2B1=1; behavior1 also clears
flags2D0 bit20. Game-facing behavior meanings remain tentative.

The full 288-byte transition matches with six register hints after minimizing
sixteen. Its scalar lifetimes and independent cursor/owner contract were
audited after minimization.

## Height, projection, and eligibility contracts

`74A98` computes terrain height from cell2F4, preferring the override cell returned
by0821D64C and falling back to map cells+12+index×4. The low nibble supplies height
in units256; high nibble1 subtracts the position's low Z byte, and2 subtracts low X.
Category22 adjusts displacement2EA by the change in ground height2BE. Categories
17–20 and23 can instead use player slot0 Y unless flags2D4 bit4000 is set.
Unless flags2C0 bit20 preserves Y, actorY becomes ground height plus displacement.
The position is copied as an aggregate into sequence drawable+1C or tile drawable+40.
Full348 bytes match, with six hints retained after minimizing seventeen.

`75508` uses direction byte2B6 and separate word scale arguments. It shifts scales
left2, multiplies them by sine(angle+64) for X and sine(angle) for Z, and uses an
arithmetic right shift12 on the wrapped word products. Projected X/Y/Z are stored
at310/312/314, retaining actorY; the prior ContactPosition is copied to318. The full
128 bytes match with two hints after minimizing sixteen. Padding travels only
through the aggregate copy, without C scalar reads.

`75FC8` first clears word304 unconditionally, so callers must supply a valid actor
even though its subsequent inline eligibility predicate handles NULL. Flags2D4
bit40, flags2D0 bit20000, or health<=0 reject the action. Otherwise78700 is called
and the callback returns1; rejected requests return0. The full88 bytes match with
five hints after minimizing eleven. The game-facing meaning of78700 remains
unconfirmed, so the action name is provisional.

## Reviewed movement/status gaps

The remaining bodies have complete semantic scratch drafts but retain assembly
and add no exact credit. `747B8` has a full736-byte baseline with six differing
bytes, all signed-load scratch choices. A broad bounded permuter found no saved
improvement; a separate search focuses on the bounce calculations. Register-hint
minimization retained that baseline while reducing140→47 hints. A local volatile
fill scalar currently controls stack-slot placement; an equivalent ordinary
scalar plus an output-only memory-placement hint was also checked. Both variants
assign the fill scalar explicitly before every actual read or `CpuSet` use.
Neither variant is promoted pending a full match and source audit.

`747B8` applies the vector at2EC and displacement2EA. With flags2D0 bit100000
clear, it subtracts2 from vertical impulse, adds that impulse to displacement,
and accumulates X/Z into velocity64/68. Ground contact either clears the vector
when factor2E8 is zero or scales X/Z by5/10 and absolute Y by factor/10. It can
emit a particle burst except for categories17,23,19,18,14,12, then invokes the
optional actor240 callback. The alternate flag100000 path halves X/Z toward zero
every fourth frame and accumulates horizontal velocity. The bit10 updates differ
between an initially stationary actor and an actor that has just stopped; no
persistent game-facing meaning is assigned to that bit.

`74EB0` advances sequence or tile playback unless byte2B3 is1, then steps impulse,
refreshes motion/cell data, calls the exact height helper, and reacts to cell
changes. Byte2B3 is also the adjacent animation selector's refresh/restart input,
so it remains a numeric latch in the shared actor view. New-cell attribute40 can
select control238 or fallback81290 and numeric substate22. Attribute80 with game
flags868 bit10 computes an impulse direction toward the cell center and prepares
collider cost/current-health fields. Its height comparison retains the observed
additional displacement2EA read after the height update. A full1624-byte scratch
baseline scores68.29%; it is not exact progress.

`75588` combines palette/status and contact-derived presentation updates. Palette
base1E0 plus offset1E6 supplies the normal bank; fields1DC and1E4 are alternate
banks selected by flags2D0 bit4000. Signed timers1E8/1EA govern flash/blink paths.
The offset1E6 is the same physical field used by adjacent sound-selection code;
this does not establish a separate sound-only runtime role. Palette transition
calls retain all five arguments of71E00; the transition manager handles categories
10 and23, as documented in the neighboring palette cohort.

Actor178 is collider134+44. In this status phase it is a visibility countdown:
with the relevant status guards clear, `(timer>>2)&1` selects drawable hidden bit0
and shadow visibility, the counter decrements, and expiry restores visibility.
This local evidence does not rename collider field44 globally. Other paths reset
numeric control states17/18, sanitize contact flags, emit particles every fourth
step of timer30C, and update target angleE9 and wrapped squared distanceF0 from
sourceDC. Player-selection indices and the player-forced target source retain
the original valid-player/storage preconditions; no new fallback is invented.
The full2624-byte status body remains a nonmatching draft.

## Remaining contiguous bodies

The remaining three functions retain assembly until their full spans match:
impulse/bounce747B8 (736), movement update74EB0 (1,624), and actor update75588 (2,624). Their behavior names are tentative review
labels, not promoted complete contracts. The prior registry-wide state request
70910 remains a separate 304-byte gap; its bounded permuter found no saved
improvement over the valid three-byte scratch-alias candidate.

## Addendum from codex/inventory-lifecycle (2026-10-15, commit 605180f2)

### Scene actor behavior frontier

This pass follows the adjacent actor placement/animation work into **08078078–0807A62C**. The first observed helper cohort ends at **08078EE8**; the following inventory entry occupies5,956 bytes through7A62C and remains under review. Names are provisional behavior descriptions, without runtime coverage claims.

The current frozen set has **eight changed TUs /1,420 reviewed bytes**, including **five newly authored exact bodies /1,132 bytes**. All five retain minimized register concessions, with no explicit r7 binding. Because196 of those bytes previously received false retained-tail credit, the net gain against the earlier inventory is **936 bytes /+0.0388 percentage points**. The other288 reviewed bytes were existing C, now typed and named without new credit. Parent integration owns promotion and one combined ROM build.

### Reviewed boundaries

Two complete independent routines were hidden in `.incbin` tails:

| Start | Reviewed end | Bytes | Evidence |
|---|---|---:|---|
|0807836C|08078430|196|Own PUSH-r4/r5/r6/LR, blink control flow, POP/BX and final alignment after sound routine return/literal pool.|
|08078550|0807864C|252|Own PUSH/save prologue, independent descriptor steering body, POP/BX and alignment after78430 return.|

Both are registered in the inventory (`symbols/progress_functions.csv`). Full-ROM stored Thumb-pointer searches and halfword-decoded B/Bcc/BL target scans found no references; independent complete function structure supports the boundaries. This adds two functions and leaves the code-byte denominator unchanged. The existing sound routine7827C is **240 bytes**, not its earlier436-byte retained span; its complete C checks exact at240. The following196-byte blink routine is now actually matched.78430 ends78550 and covers288 bytes, rather than claiming the252-byte independent successor.

### Proximity and script helpers:78078–7836C

78078 checks players0 through the index held in02000544, skipping absent player pointers. It returns true when the actor/player absolute wrapped signed16 X and Z differences are each at most255. If no player qualifies, it checks the first registered kind4 contact object's position atC with the same test. Y is ignored. The full232-byte routine is exact; depin reduced7→1 hint.

The proximity source uses `ContactPackedVector` and initializes x/z before reading those same members. It never reads the uninitialized y member or the entire storage word. The compiler's storage-unit RMW may preserve unspecified neighboring bits; this is a valid member initialization, not an uninitialized scalar/union read.

78160 invokes the low16 script ID at actor1F4 only when flags2D0 bit80000 is clear, the terrain cell has none of flags43, unsigned ground height2BE minus signed actor Y is at most256, and there is no dynamic cell override. The first terrain lookup prefers that override, otherwise using the map's four-byte base cell records. The12 script values are actor XYZ, words1FC/200/204, -1,0, category98, paletteOffset1E6, parameter2D8 and-1. `ScriptUtilityArguments.count` is initialized as its own bitfield member, and the values pointer is initialized before the call. The full260-byte routine is exact; depin9→2 hints.

78264 invokes the low16 action-script ID1F0 with a NULL argument block when nonzero; its24 old bytes remain exact.7827C dispatches reaction sounds from category98, paletteOffset1E6 and flags2D4 bits6. Its240 old bytes remain exact. The higher-level interpretation of the1E6 selector remains tentative despite sharing the registry paletteOffset storage.

### Drawable blink and steering:7836C–78700

7836C chooses a signed cadence from08605C20: index2 at age≤10, index1 at11..30, index0 above30. At cadence boundaries it toggles drawable flag1, using sequence flags0 for drawable kind0 and tile flags28 otherwise. Age>60 always clears the flag and returns true; younger ages return false. Full196 bytes match; depin31→4 hints.

78430 uses the descriptor's destination1C to derive untruncated signed X/Z differences. Actor flags2D4 bit1000 select a special stopping rule: descriptor flag1 must be absent and the signed interpretation of wrapped squared distance must be≤4000. Otherwise the descriptor's positionC is compared to independent unsigned X/Z tolerances. Outside the stopping rule, nonzero destination deltas set heading59 and produce sine-table acceleration from the fifth speed argument. Components with absolute magnitude>10 are added to velocity64/68; both velocity halfwords are stored even when unchanged. Full288 bytes match; depin41→8 hints.

The steering source explicitly uses unsigned products for wrapped32 multiplication and casts to signed for the observed comparisons. Negative fixed-point scaling expresses wrapped negation in unsigned arithmetic before its signed arithmetic shift, retaining original INT_MIN machine behavior without signed-negation overflow. Initialized speed lifetimes keep natural load-scratch allocation; no handwritten instructions are present.

78550 is a complete private ordinary-C descriptor acceleration draft. It uses wrapped signed16 destination deltas, positional tolerance checks, a byte heading, and adds scaled sine components plus actor halfwords310/314 to velocity. Its full252-byte draft differs only in two addition-order swaps (98.4% raw fuzzy). A bounded15-minute permuter search is running against a reviewed ROM reference. Hidden entries are raw incbins in split output, so the private reference is generated from original ROM halfwords with Thumb code markers and the two literal pools/data padding marked separately; the initial data-only reference was rejected because its score was misleading. No candidate from that invalid setup is credited.

7864C uses wrapped signed16 destination deltas. Outside the descriptor's X/Z tolerances, it compares target/current facing quadrants derived from heading+32. A different quadrant writes requestedDirection3CD=(targetQuadrant*2+3)&7 and returns true. Otherwise it returns false. Full156 bytes match; depin8→4 hints.

786E8 initializes the actor's shadow at320 using its position5C and zero flags. Its24 old bytes remain exact, with a typed actor formal.

### Remaining contiguous work and verification

The adjacent78700–78EE8 helpers configure colliders and process contact/reaction state; their full contracts are still being reviewed. The old108-byte78B48 body remains existing C and is not part of this frozen review count. The large78EE8 inventory span is not yet promoted from private assembly.

Fast full-extent proofs are in the worker's private `build/behavior-{near,script,action-script,sound,blink,accelerate,turn,shadow}-final.json`. No worker complete ROM build was run. The earlier72854 terrain and72F4C contact-evaluator semantic drafts, and6D184/6D438/6D740 route drafts, remain private and uncredited. Their evidence and search history are retained in the earlier subsystem notes and build artifacts.
