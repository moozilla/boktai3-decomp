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
