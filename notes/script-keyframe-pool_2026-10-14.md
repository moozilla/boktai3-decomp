# Script keyframe pool

`08055678–08055B74` is a complete thirteen-function cohort /1,276 bytes, with
four new exact functions /672 bytes (+0.0278 percentage points). Nine existing
helpers, including the260-byte updater, were typed/named without new credit.
The shared layout is `include/script_keyframe_pool.h`. These are static
contracts, with no new execution coverage or identified in-game object.

Manager `gUnk_02000498` has capacity/count18/19 and0xBC entries at1C. The creator
uses list8, capacity from m (default1), and allocates1C+capacity*BC. Each entry
contains a copied32-byte resource header18, a0x60-byte keyframe animation38,
tag98, moving9A, conditional visibility9C, hidden override9E, movement frame/
durationA0/A2, positionA4, originAC and targetB4. Two established partial views
of the animation are retained in a named union: ProjectedDisplayEntry supplies
flags and position; DisplayKeyframeState supplies priority. This avoids assuming
that all animation ownership fields have a universal layout.

Lookup requires n and compares the full script value with the halfword tag,
reloading the manager after parsing. Setters select a keyframe with p, mirror
with f, priority with p, or hide/show. Hide/show update both animation flag1 and
the hidden override when conditional visibility is enabled. The updater then
shows the animation only when that override is clear and GameState.unk14 is
nonzero. The meaning of unk14 remains uncertain.

The movement setter optionally reads p XYZ and f duration, copies position to
origin, resets frame, and enables movement. It can reuse an existing target or
duration when keywords are absent. Update increments frame, copies target at
completion, otherwise computes signed Div interpolation on all three axes, and
copies position into the animation. With duration0 the first tick completes
before any division.

Entry creation requires d, looks up extensionCB05, copies and relocates its
32-byte header, reads i frame(default0), m mode(1 selects flag16), R priority
(default2), p position(default zero), and A conditional visibility(default0).
The eight-argument keyframe constructor0821983C receives frame, mode, priority,
zero,60 and the stored position pointer. The hidden override is cleared; count
increments after required parsing succeeds. The manager updates all entries,
unlinks their animations on destruction, and clears the singleton.
