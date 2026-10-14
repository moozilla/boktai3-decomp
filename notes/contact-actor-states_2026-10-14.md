# Contact actor states and lifecycle, 08042E18–08043F64

The gameplay identity remains tentative. The table at 086058FC establishes the
state mapping below; descriptions follow code and data flow, without new emulator
coverage claims. Eight new exact functions add **0.082 percentage points**
(1,972 bytes). Existing task factory, destructor, and clear request gain names
and types but no additional exact credit.

| State | Address | Observed behavior | C status |
|---|---|---|---|
| 0 | 08042D58 | Advance along heading, then enter state 1 after index+2 frames | Already exact |
| 1 | 08042F44 | Jitter a point around selected target, or choose random heading; move toward it and approach desired height | 66.9% guarded draft |
| 2 | 08042E18 | Face selected target; check squared distance every eight frames; enter state 1 if beyond 142884, state 4 after frame 63 | New exact, 300 bytes |
| 3 | 0804331C | Random heading, refreshed every 16–31 frames; move for 32 frames then enter state 2 | New exact, 396 bytes, no hints |
| 4 | 080434A8 | Save origin, face target, use triangular frame-scaled displacement relative to origin; update both colliders and refresh resource collider | New exact, 376 bytes |
| 5 | 08043620 | Disable health contact, decelerate along heading, select reaction graphics, flash sprite; return to state 1 after frame 19 | New exact, 284 bytes |
| 6 | 0804373C | Update retreat-center terrain height, choose nearby point, seek it at speed 16; enter state 7 within squared distance 16383 | 51.7% guarded draft |
| 7 | 08043A50 | Disable health contact and wait; can enter state 8 when hide requested | New exact, 72 bytes |
| 8 | 08043A98 | Disable health contact and continue sequence/frame updates | New exact, 52 bytes |

The timer at slot+9 counts down to retargeting. Pool mode 2 assigns target 1 to
indices 0–7 and target 0 to 8–15; mode 1 selects target 0; other modes select -1.
Positions at +14, destination +1C, and burst origin +24 are packed halfwords.
The height fields at +C/+E are current target height and desired height, while
byte +7 is the steering countdown. All offsets and the 0x16C slot stride are
shared in `include/contact_actor.h`.

Pool_Update at 08043ACC is new exact C (344 bytes, no hints). After lifetime 599,
it requests clearing, plays song EA once, and starts fade mode 2. It updates
all sixteen slots through the state table, then display playback. Sixteen slots
in state 7, or lifetime beyond 1199, requests hiding; the next manager update
hides the pool directly. State 8 is present in the table, but this manager path
does not prove it is reached. Fade modes raise to 63, lower to zero, or raise to
32; -1 stops blending. The buffer at +1700 has 32 bytes within the confirmed
0x1720 task allocation.

Pool_Initialize at 08043C4C is new exact C (148 bytes). It loads the sequence
resource (extension hash 922E, name hash 5BB7), initializes every slot, and
points targets at player positions +30. ScriptProc_Start is the historical name
of the resource lookup; it does not start a script process. The pool's +30 is a
retreat-center position, +38 is the sequence resource, and +24 is lifetime.
Existing hide/sequence consumers were updated and rechecked. RequestClear at
08043F5C simply sets lifetime to 600. Create at 08043CE0 allocates list 8 and
0x1720 bytes; it was already exact before this batch.

Pool_Start at 08043D2C is a **72.7% guarded draft**, original 560 bytes versus
576 compiled. It initializes the ring of headings, target split, source-relative
positions, randomized height/steering timers, sprites/shadows and colliders.
The extra parameter stored at slot+12 remains unnamed. It is not credited.

Hints were minimized before accepting each exact function. Tracking reduced
49 to 16 hints, burst 33 to 16, reaction 12 to 3, wait/inactive 2 to 1 each,
initialization 3 to 2. Random walk and manager update became entirely unpinned.
No function binds r7 or contains handwritten instructions. Tracking uses a
clobber-only empty barrier to let agbcc allocate a natural r7 scratch pointer.

Local failed attempts are retained in ignored build/candidates and build/contact-*
checkpoints. Tracking moved from 80.8% through load ordering and branch-layout
fixes to full 300-byte equality. Roaming retains 988 versus 984 bytes; retreat
792 versus 788. Retreat also has an apparently unused original r8 constant load
0403B714: investigate its origin rather than introducing an unexplained forced
instruction. Start attempts with separately traversed health pointers, pinned
random-table registers, and a volatile index enlarged the stack and regressed;
the simpler reviewed draft is retained.

The combined ROM build and integration checks are recorded in docs/HANDOFF.md.
Continue at 08043F64, the next actor's collision callback. The user replaced the
usage stop threshold with **60% remaining**, while retaining the 25% exact target.
