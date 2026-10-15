# Actor control phase and animation transitions

The contiguous82648–83550 group continues the preceding reaction callbacks. It
contains contact admission, heading/tick animation maintenance, persistent and
shadow-return animation callbacks, motion convergence, scripted response setup,
and state controllers. Names remain tentative and static evidence does not
establish runtime coverage.

Actor3D0 points at a context whose flags word3D4, pulse interval3DA, elapsed
limit3DC, saved direction3DD and wrapped byte clock fields3E6/3E7 are consumed
by this group. The narrow shared view records those observed offsets without
assigning allocation ownership or the global tick's broader meaning.

82B78 matches120 ordinary-C bytes and is a whole-body clone of the existing
82BF0 function. It consumes prepared2B2, selects directional animation1 and
saves direction59 at context3DD; later heading changes repeat that operation.
An unchanged heading clears animation-ended2B3 using the consumed false value.
82C68 matches64 ordinary-C bytes: prepared2B2 selects directional animation15
with flags0, and every call clears animation-ended2B3.82CA8 matches84 ordinary-C
bytes: prepared2B2 selects animation17/flags0; when animation-ended2B3 is set it
sets actor2B0 and enables shadow320. These three helpers recover268 new bytes
(+0.0111 percentage points) without register concessions.

The larger front and state-control bodies remain assembly/private until their
full reviewed extents match. Existing82BF0/82CFC/82D20 already had exact C and
receive no new matching credit from this review.

829C0 now matches its complete172-byte extent (register-pinned): it rejects
unavailable actors and source-contact flag0800, applies the shared contact
reaction, advances the1024-entry RNG cursor, subtracts the sample's low four
bits from destination power40 with a signed floor20, and clears context flag8.
The function does not add a null-actor fallback: as in assembly, a null actor
passes the availability helper and is invalid if execution reaches the final
context access. These caller preconditions are separate from the helper's gate.
Together the four newly recovered bodies add440 bytes (+0.0182 percentage
points):268 ordinary and172 register-pinned.

82A6C matches268 register-pinned bytes. Preparation snapshots byte4F4 from the
first global player pointer, selects directional animation1, and saves heading.
A zero remainder of actor halfword2CA modulo context pulseInterval clears bit4
in the secondary collider. This call requires a nonzero pulseInterval. Elapsed
clock time wraps by256 and is stored as a byte; exceeding maxElapsed sets2B0,
otherwise heading changes restart animation1 and unchanged headings clear2B3.
The observed clock offset is not assigned a broader gameplay meaning.

83118 matches112 register-pinned bytes: it consumes state-changed2B1, restores
heading from E9 only when that flag was set, then unconditionally sets animation
state/phase/behavior0/2/0, clears the counter, marks state changed again, and
increments the counter to1. The clear-then-increment sequence is retained.

83188 matches156 register-pinned bytes. Consuming state-changed2B1 installs
persistent callback82C68 with control kind34, resets the common control fields,
and disables the shadow. Every call then invokes callback270 and increments
counter2AC only when its low-byte result is zero. The callback's effect is not
inferred from its result alone. The seven new bodies now recover976 bytes
(+0.0404 percentage points):268 ordinary and708 register-pinned.

83224 matches304 register-pinned bytes. State-changed2B1 installs shadow-return
callback82CA8/kind35, clears flag100000 in2D0, and initializes secondary motion:
X/Z use the sine table at heading+64/heading, scaled by4 then divided toward
zero by4096; Y is cleared before being set to10. If2B0 is set and signed2EA is
below the context halfword3D8 minus20, it calls84044; otherwise it increments
the counter. This is a threshold relation, not yet a confirmed physical height.
The eight new bodies total1280 bytes (+0.0530 percentage points), of which268
are ordinary and1012 register-pinned.

82F70 matches424 register-pinned bytes. Preparation selects animation10, plays
category event sound and sets the primary collider's flag4. After the drawable
frame-window predicate succeeds, it sets2B0 and optionally sends an initialized
eight-value script packet (signed XYZ,1FC/200/204/2E0, actor ID). It frees the
category resource when2D4 lacks4000 and category is outside11–13; it calls the
terrain script when2CC lacks0400. Neither test clears its flags. It then clears
AA, installs default response controlFC90/kind16, sets animation1/0/2 and
disables the shadow. All nine new bodies total1704 bytes (+0.0705 percentage
points):268 ordinary and1436 register-pinned.

82D80 matches496 register-pinned bytes. Preparation sets2D4 bit20, animation3,
and secondary-collider flag4. Every call sets2D0 bit100000, takes signed
(310−318)/4 and (314−31C)/4 with truncation toward zero, clamps each to±200,
then adds them to motion X/Z and the saved318/31C values. A zero pair means
both truncated deltas are zero, not necessarily equality of the original values.
It then sets2B0, clears2D4 bit20 and2CC bit80, and clears context phase3E2.
Actor EB bit8 selects animation0/2/0 plus player-facing setup, or sets EB bit8
and animation0/0/3; both paths set2D4 bit100 and call83E34. An intermediate
hard-register trial incorrectly omitted one whole path and was rejected. The
accepted whole496 bytes include both paths and use no explicit r7 binding.

All entries in82648–83550 now have complete static semantic review. The two
remaining assembly bodies have complete C drafts:83354 is published behind NONMATCHING, while82648 remains private.82648(888-byte target,
880-byte draft/53.8% fuzzy) admits destination kind1 and performs two actor
availability checks, conditionally records contact ID4 at actor304, clears2D0
bit01000000 and calls the shared contact operation. Field1E6 values1 or12–15
allow health recovery from contact3E, capped at default healthB0; colliderA3E
and actor2E6 record the applied amount. Global4800 flags, actor2E0 and optional
selection18 gate the status/number display. Its initialized eight-byte zero
vector is supplied to DamageNumberDisplay_Show. Final control logic clears
contextflag8 and, when flag4 is absent, installs FC90/kind16, selecting the
EB-bit8-dependent animation path. Null actor is a caller-precondition violation
if these later accesses are reached; the availability helper adds no fallback.
Field1E6's broader meaning remains tentative despite its current shared name.

83354(508-byte target and full508-byte/98.4% draft) restores default control228
on state entry, clears eight context bytes42C, saves an RNG byte at context3EC,
and initializes sine-based secondary motion X/Z divided toward zero by128.
It recomputes those components while signed counter2AC≤5. A zero squared
magnitude clears the full eight-byte motion vector, runs83E34/player-facing
setup, and installs animation0/2/0; every call increments the counter. The C
sum uses unsigned32-bit products/addition to preserve the assembly's wrapping
arithmetic for extreme signed-halfword inputs. Six differing bytes are two
LDRSH scratch-register pairs, in a fresh frozen four-hour/j1 permuter queue
(run-g042m0h3). No draft or permuter score contributes exact credit.

The ten new exact bodies in this section total2200 bytes (+0.0911 percentage
points):268 ordinary and1932 register-pinned. The adjoining motion/ground/sway
cohort at83550–83AEC is handled separately and includes its own context view.
