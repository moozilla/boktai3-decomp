# Twelve-slot bouncing objects (080688D0–08069964)

The provisional `BouncingObject` name describes observed contact impulses and
movement, not a confirmed game object identity. This family uses singleton
`0200014C`, distinct from the preceding anchored-particle singleton `02000134`.
No runtime coverage or specific screen identity is claimed.

## Observed ownership

The task has graphics at `18`, a sequence-resource pointer at `34`, and twelve
`14C`-byte records at `38`. Its used mask is at `FC8`, with a stopping word at
`FCC`. Each record contains a sprite at `0`, animation state at `2C`, colliders
at `3C/90`, registration storage at `E4`, velocity at `124`, origin at `12C`,
flags at `134`, state at `136`, signed index at `137`, an entry flag at `138`,
collision delay at `139`, timer at `13A`, map cell at `13C`, spawn parameters
at `13E/140`, animation state at `142/143`, configuration at `144`, initial timer
at `148`, and state-frame counter at `14A`. These layouts are shared only where
multiple actual consumers establish the fields.

The allocator selects the first clear bit among twelve entries, sets the bit,
and stores the signed slot index. Its exact 88-byte implementation retains one
local register binding. Creation, physics, and script parsing remain separate
review tasks at this snapshot.

## Contact and collision setup

`688D0` subtracts contact positions, quantizes the horizontal direction, and
applies an impulse. A positive vertical difference replaces velocity with a
small horizontal component and Y=-4. Otherwise it adds a larger horizontal
component and Y=32. It raises position Y by one, sets collision delay to twelve,
and plays sound166. The vector conversion truncates fixed-point results toward
zero. Its 364-byte body is exact ordinary C after removing all six initial
register hints.

`68A78` initializes a body collider with size32 and offset `(0,32,0)`, then a
removal collider with size `(64,128,64)` and offset `(0,128,0)`. The callbacks
are the impulse routine and the timer/flags contact routine at68A3C. The two
colliders use flags2003 and4001. A packed aggregate constructs the complete
coordinate records, including padding, before typed field changes; it does not
read an uninitialized scalar. The full276-byte function is ordinary exact C.

## Adjacent anchored-particle lifecycle

The preceding eight-slot particle pool is documented in
[SCENE_ACTOR_CONTROL_PARTICLES.md](SCENE_ACTOR_CONTROL_PARTICLES.md). Its
104-byte dispatcher and84-byte destructor at68784/687EC are now exact, with
minimized lifetime constraints. The644-byte spawner at68500 remains private
nonmatching C, with a frozen permuter search. Its shared sprite-offset helper
has three arguments: object, X offset, and Y offset. Both offsets are -8 here.
A two-argument declaration was corrected before restarting the search.

The new twelve-slot cohort currently adds728 exact bytes (364+276+88), of which
640 are ordinary C and88 register-pinned. The adjacent anchored allocator and
lifecycle add268 bytes with register concessions. Exact checks cover each full
reviewed extent; batch-wide ROM validation is performed by the integrator.

## Timer removal, heading query, and pool lifecycle

`68A3C` advances the contact timer/flags path; `69214` removes an expired slot
and writes the signed index sentinel. The pool updater69510 walks occupied
entries while respecting the engine pause and stopping flags. Destruction695C8
removes the sprite and its registration record for every occupied slot, clears the
singleton, and returns zero. Typed initialize/ensure/reset helpers preserve
the actual task callback return contract.

`696C0` returns the heading from the input position to the first eligible
occupied entry, rather than searching for the nearest entry. It excludes
negative indices, zero timers and state two, and compares the signed wrapped
horizontal squared distance against102399. `69788` answers whether a signed
slot index is active. These additions contribute776 bytes:468 ordinary C and
308 register-pinned. This includes the previously hidden64-byte query as a
newly matched explicit boundary; the large physics/creator and script bodies
remain separate assembly-backed gaps. The close584-byte creator has a frozen
long permuter run from a two-register-byte difference.
