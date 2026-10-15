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
