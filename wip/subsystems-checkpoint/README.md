# Unmatched subsystem continuation sources

These are unfinished candidates, not production C or exact progress.
Do not merge this directory into main. Parent main matching baseline after this
batch: 15.315%. Sources use the accepted background layout plus local affine.h
(the provisional affine name was changed to scrolling in production).

- 08001768: normal background frame update, natural candidate 848 bytes versus
  872 target span. Broad pinning worsened it; declaration permutations did not help.
- 08001EE0: map-list script loader; natural version spilled a long-lived scalar.
  Fixed registers reduced the spill but changed lifetimes around argument setup.
  Current draft remains unverified and must not be accepted by similarity.
- 08002024: single-map script loader, followed by a hidden function at 080020BC.
  Current inventory span includes that hidden body; split its reviewed boundary
  before ever claiming the whole span. Inline tryIndex currently escapes to stack.
- 0800211C: tile-loading script command; loop rotation and literal placement remain.
- 080021DC: command queue, six differing bytes: moving source pointer from ip to
  r6 occurs before rather than after two invariant-address instructions. Attempts
  to materialize count/command pointers changed alias-driven reloads or order.
- 08002408: scrolling frame update, 596-byte candidate versus 592-byte body.
  Fixed-point helper result moves, camera adjustment order and fade-field loads.
  Broad register pinning made it worse; do not repeat it blindly.
- 08219B24: input polling, candidate 176 versus 180 target. Logic and most register
  assignments align; loading 0x3FF directly into r6 replaces target's r0 load/copy.
  A 120-second two-worker permuter search improved score 255 to 245, no exact match.

Names are provisional static roles. No emulator-derived gameplay identification.
