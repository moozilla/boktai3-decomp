# Scene actor registry (tentative)

The reviewed worker span is `08070FA8` through `08071E00` exclusive. The final
address begins an independent palette transition helper. The manager singleton
is `020004A8`, allocated on task list 8 with size `0x288`. Global `020004AC` is
a linked list of nodes `{next, actor}`; traversal ends at a node with a null
actor. Destruction advances the list before invoking an actor's destroy
callback and freeing its context. The callback argument and heap object are
actor field `0x3D0`; callbacks are at `0x258/0x25C`.

The shared narrow view is `scene_actor_registry.h`. Actor `0x98` is named
`category` tentatively: consumers distinguish several fixed values and actual
transition state bytes occur elsewhere. Offset `0x9C` is signed health, `0x50`
is an ID, `0x5C` is position, and `0x2FA` is compared with `GameState.region`.
Flags at `0x2CC/0x2D0/0x2D4/0x2D6` retain offset names until their several bit
contracts are fully reviewed. The palette bank fields at `0x1DE/0x1E0/0x1E6`
and the drawable variants are based on observed calls to the 32-byte RGB15
bank selector; their combination roles remain tentative.

`71470` advances nonpositive-health actors whose `flags2CC` bit 1 is set,
only while engine mask `0x4800` is active and keyword `08605C18` is absent.
`71CB0` updates game-state flag `0x20` every 16 manager frames according to
`OrbitBackground_IsEnabled`. The manager updater calls the adjacent registry
steps, clears manager flags `0xC000`, increments its frame and returns zero.
Destruction frees actor contexts and the list, clears an adjacent subsystem
and resets the singleton; it returns zero. Reset simply clears the singleton.

Initialization stores the low 16 bits of its resource argument as a word at
`0x24`, clears frame/fields `0x32/0x62`, sets mode 3, calls `6FAD4`, publishes
the singleton and clears the two-word subsystem at `0x280`. Its third formal
is passed by the creator but unused. A reviewed ABI wrinkle needs integrator
coordination: helper `73BFC` retains its input address in r0 while clearing the
two words, and the creator tests the initializer's result as signed-negative.
The initializer explicitly forwards that retained pointer as a signed status;
the helper's existing void C definition must be aligned to return its input
pointer with identical bytes. There is no authored fall-through return.

The larger `70FA8` draft traverses callbacks, handles flagged removal,
conditionally emits three packed-vector particles and manages two palette
transition categories. It remains an ignored nonmatching draft, with no exact
credit. `7171C` now resets its embedded route, samples terrain height and slope, copies
the grounded position into either drawable, clears both motion vectors and
restores its base palette bank. A flagged reset also clears the embedded
control timer/counter/mode. The extended reset-phase layout is in
`scene_actor_reset.h`; it is kept separate while the adjacent control consumers
consolidate the common actor view. Other reset routines remain in progress.

Current exact new progress: `71470` 116 bytes, `71D60` 60 bytes and `71D9C`
88 bytes, totaling 264 ordinary-C bytes (about 0.0109 percentage points).
Existing 248 bytes now have typed contracts without new credit. The retained
one hint in the initial inactive-actor draft was removed by `depin.py --write`.
All eight reviewed extents pass private checks; the parent owns combined ROM
validation and shared-header consolidation with the adjoining worker.

The adjoining control cohort establishes actor motion at `0x64`, fields
`0xA8/0x104`, flags `0x114/0x2C0`, control pointer slots `0x21C/0x22C/0x5F4`,
and the transition bytes/counter at `0x2A8` onward. These verified additions
from the neighboring worker are included in the shared view. Callback pointers
remain opaque until indirect dispatch establishes the complete ABI.

`7171C` adds 444 exact bytes (about 0.0184 percentage points), all counted as
register-pinned. `depin.py --write` reduced 46 hints to 14. All nine extents
using the expanded shared header pass; proof is `build/registry-reset-final.json`.

`714E4` adds the collider reset variant (568 exact bytes, about 0.0235
percentage points). Categories 9, 18, 19 and 22 skip the route-placement call
sequence; all categories sample terrain, restore palette and clear movement.
This variant copies the saved direction, synchronizes both colliders and clears
collider flag 4 plus its field at offset 0x44. `depin.py --write` reduced 34
hints to 11. It remains in the register-pinned category. Expanding the phase
view for the next defaults-restoration routine preserves both reset matches.

`718D8` restores saved actor defaults after the collider reset. Its two paths
both restore flags, direction, behavior bytes, control pointer, signed health
(clamped at zero), and common movement/control state. The path with a nonnull
object additionally resets the sequence block at `0x460`, clears its surrounding
state, and recreates attachment kinds 1 and 3 using the actor ID. The final
optional callback is at `0x248`; the earlier phase-view label at `0x468` was
provisional and has been corrected to an unknown word. The initial control-mode
clear is at `0x42A`, distinct from the later byte clears at `0x464..0x467`.
`72188` takes no arguments; incidental r0 values at these calls are not inputs.
`12E084` consumes only its first argument, but the caller retains the observed
resource/parameter arguments in the local prototype. The two attachment calls
forward the first returned handle directly into the second.

The defaults restoration is exact over 984 bytes (about 0.0407 percentage
points). First `depin.py --write` reduced 177 hints to 66; after replacing raw
stores with named fields and removing unused address assignments, a second
pass reduced those to 51. Explicit offset temporaries remain where the old
compiler's pointer lifetimes affect allocation. All three reset routines
(1,996 bytes, register-pinned) pass their full reviewed extents against the
expanded phase view: `build/registry-reset-cohort-final.json`.

The remaining `70FA8` gap has a concrete packing diagnosis: creating addresses
of stack unions before their first packed-field assignments selects halfword
memory stores in this compiler. Assigning packed fields first, then creating
the addresses, produces the required word-mask stores and the target stack
order (velocity `sp+0xC`, bounds `sp+0x14`, position `sp+0x1C`). The ignored
`build/registry-tick-delayed-address.c` preserves this version; it is not exact.
The unsigned bitfield view in the common header need not change; this private
candidate uses a local signed packed-vector union. The older draft and each
allocation experiment remain under ignored `build/` for later comparison.
