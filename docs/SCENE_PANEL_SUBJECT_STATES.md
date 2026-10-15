# Scene panel subject animation states

The reviewed `0805CD64–0805D6C4` span has six new exact functions, 2,400 bytes
(+0.0994 percentage points). They occupy states one through six in the eight
entry table at 08605AF8. State zero is the existing no-op; state seven and the
subject driver are described in SCENE_PANEL_SUBJECT_TAIL.md.

The integrated subject layout is in scene_panel_subject.h. Drawable18 contains
frame28 and position34; graphics44 precede home60, collider68, sequenceBC and
its resourceCC. This reuses the established sequence and drawable types.
The common inline playback helper extracts frame from command bits6+, compares
command bits4/5 against sequence flags1/2 to select drawable flags4/8, then
advances elapsed time. Flag4 reverses playback; flag8 holds at an endpoint.
Crossing an endpoint returns one even when playback loops. The new command's
low four bits set weight; byte duration is `(speed * weight) >> 6`, with zero
replaced by one. Volatile command views retain the observed halfword reads.

| State | Handler | Behavior beyond playback |
| --- | --- | --- |
| 1 | CD64 appearing | Decrement appearance delay and skip other work while nonzero. Otherwise show drawable, clear collider flag4, set priority2, select sequence0. On endpoint refresh collider and enter state2. |
| 2 | CECC settled | Select sequence1 and palette bank24C. |
| 3 | CFE8 activating | Select sequence2. At frame5/elapsed0 select bank24D. For nonzero kind, frames after5 select24E when EE is nonzero, otherwise24D. Endpoint sets changedF0 and enters state4. |
| 4 | D17C active | Select sequence3; bank24E when EE is nonzero, otherwise24D. |
| 5 | D2B0 departing | Select sequence4. Nonzero kind selects bank24D at frame5/elapsed0 and24C at frame6/elapsed0; endpoint clears contact progress, sets E2 to90, enters state6. Zero kind selects24C at frame5/elapsed0; endpoint clears progress and enters state2 directly. |
| 6 | D584 reset delay | Play sequence1, decrement E2. At zero clear positionReset, restore all eight home-position bytes and enter state2. Otherwise invoke the position jitter helper. The decrement wraps as a halfword. |

The palette choices are inputs to 08215284, which selects a 32-byte bank of
sixteen RGB15 colors. They are not sound IDs. Entering state3/state5 plays
songs FB/FC in the state setter. Completing state3 sets changedF0, consumed by
the parent's state4 scan for song294 and a presentation callback. Contact
progress reaches its limit in the contact helper before selecting state3;
this links the collider, subject state machine and parent notification.

The inline helper retains eleven jointly minimized register/empty-asm hints.
CFE8 additionally retains two local hints; D2B0 one. All six including callers
of the shared helper are marked as register-pinned concessions. The joint
minimizer checks full extents across the four distinct initial helper users;
removing any remaining helper hint breaks at least one. Per-file depin was
also run before promotion. All values passed to empty asm are defined; there
are no instruction bodies or explicit r7 bindings. The temporary hints are
separate from the established field/command semantics.
