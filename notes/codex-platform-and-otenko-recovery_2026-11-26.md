# Moving platform and Otenko source recovery

Tags: #actors #types #matching #phase3

## Moving platforms

The platform slot now declares its alternating trail phase and embeds
the primary and four neighboring terrain overrides. The follow-cell
loop at 08101B1C indexes the neighboring records directly. The
occupancy check at 08101E20 uses the slot array and the override's cell
index. Keeping separate indexed field expressions preserves the two
induction values. Capturing a slot pointer first causes agbcc to carry
a different base through the loop.

The shared actor and player layouts replace private copies for movement
trails, lift travel, attachment, collision bounds, and destruction.
Scope confirmed that 08102D20 reads the speed halfword at 0xBE when
clamping the attachment. The shadow bit update at 081029F8 works
directly through the player shadow field and removes a single-pass loop.

The source goes from 68 to 67 diagnostic lines and retains 65 constant
uses. All changed units match, and all 30 dependents matched after
typing the registration records. Direct substitutions at 56 generic
flag sites differ from the ROM, so those sites remain visible.

At 08103110 plain follow assignment leaves four differing instructions.
Scope showed that a competing zero takes r5 before the follow mode. The
diagnostic priority increase `AGBCC_PRI=follow:+407` confirms the choice.
Plain branch and switch forms, a local attachment helper, and chained
state resets still differ. Splitting the sprite and route pointers
changes PRE and spilling around the embedded route fields. The local
search is in `build/matchsearch/MovingPlatform_CreateFromScript`.

## Otenko

Otenko's command handlers use the shared state actor for the short
message states. The timer writes at 081CE9DC and 081CEA54 match with
literal zero. Placement, playback, projection and command dispatch
use the 0x384-byte Otenko actor. It embeds the scene body at 0x5C,
display at 0xD4, message receiver at 0x168, and ground effect at 0x344.
The animation callback at 0x2E4 has the state-change query's argument
and return types.

Typed fields support direct resets at 081D010C, 081D07D8, 081D0C04,
081D10D8, and 081D116C. The projection effect at 081D0928 writes the
particle z velocity directly and reads the work palette and
projection through their fields. The 0x1FC-byte transition task uses
an embedded sprite, animation resource, and particle array. Its ring
initializer at 081D15EC writes the particle active flag and task
state directly.

The source goes from 57 to 52 diagnostic lines and 96 to 68 constant
uses. Stock checks match in every edited unit. The actor and transition
headers passed their dependent checks.

- At 081D010C the remaining single-pass loop controls whether the
  state 0x24 or zero materializes first. Assignment chaining and byte
  and word local types still differ by instruction order.
- At 081D07D8 the plain block changes local allocation of the projection
  pointer and zero. The ROM has two height stores. A single height
  assignment shortens the function by eight bytes.
- At 081D0864 the typed bobbing update still relies on the original
  shared storage for the flag and counter. The separate predicate
  changes mask order and shifts spill selection.
- At 081D01C8 the work palette binding computes the graphics base at
  0xE0 and stores at +0xC. Writing the deep field directly computes
  0xEC instead. The typed binding retains the original operation.
