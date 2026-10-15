# Proximity emitter, 08051A3C–08052130

All eleven functions match across1,780 contiguous bytes. Seven new functions add
0.0611 percentage points (1,476 bytes):104 bytes without hints and1,372 with
minimized concessions. Existing growth/tick/destruction/creation helpers were
typed, named and checked over their complete extents.

A list9 task allocates0x268 bytes. Shared proximity_emitter.h records two0x48-byte
sprite/graphics packets at18/60, nearby A8, state-frame AA, origin AC,
state callback B4, particle resource B8, ring index BD, emission frame BE,
eight0x34-byte particles C0 and integer enter/leave script IDs260/264.
Each particle has world position18, velocity28/2A/2C, active30 and frame32.
Particle display bytes F/10 remain unnamed; the grow animation also writes
secondary sprite bytes6/8/9 without claiming their exact renderer semantics.

The range repeatedly consumes TetherActor_GetPosition and requests the tether
actor palette blend. IsTetherNearby checks absolute X/Z deltas<=127 and computes
a squared-radius comparison against16383. Its original return is **not a clean
boolean**: when both axes pass but the radius fails, R0 still contains the
position pointer from GetPosition. The matching C preserves nonvoid fallthrough.
Consequently observed callers treat the whole127-unit box as nonzero. Do not
silently replace this helper with a conventional circle predicate.

TickActive enters only when the proximity result is nonzero and player+255 is1;
it calls the optional enter script, requests blend1 and sound198. Leaving calls
the optional leave script, requests blend0 and sound199. It emits once per8
frames while inactive and per4 while active. Script IDs are repeatable callbacks,
not pointers or consumed handles. The grow state lasts64 ticks, changes secondary
sprite rendering parameters and transitions to a15-tick rise. Rise emits every
other frame, then returns to active state.

Particles live16 update frames. They integrate three signed-halfword velocities,
alternate effect keyframes2/3 every4 frames, then hide and clear active. Emission
reuses an eight-slot ring, draws four RNG samples in XYZ/vertical-speed order,
places X/Z in[-127,128] around the origin and Y in[0,63], and sets upward velocity
16..31. The RNG index is stored once after the four draws. Particle initialization
uses resource1C1E, effect offsets(-4,-4), keyframe2 and fixed display parameters.

Initializer p gives XYZ, but the supplied Y is replaced by terrain height.
Surface1 subtracts low Z and surface2 subtracts low X, using the same terrain
lookup and base-map fallback as the preceding breakable prop. Primary/secondary
sprites share resourceBA8, palette selectionsA2/A0 and priority3; the primary begins
30 units above the origin. Nonzero m selects grow animation and sound15E;
otherwise active state starts immediately. Keywords i/o provide optional
enter/leave script IDs. Initialization queries proximity twice as in the original,
then sets nearby and requests its corresponding blend. Exact gameplay identity
remains provisional; no new runtime coverage is claimed.

Palette evidence: display-track update54EC0 interpolates sixteen RGB15 colors
from the 32-byte banks of030042E4. The helper is now named
AnimData_SelectPaletteBank (formerly AnimData_SelectTiles), and the marker fields
are named paletteBase. These names replace the earlier tile interpretation.
