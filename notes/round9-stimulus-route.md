# Scene actor stimulus routing (0807F130)

Tags: #matching #scene-actors

The guarded callback rejects a negative overlap result and retains the low byte
for tagged-position resolution. When the control timer expires it clears the
control counter, halfword and timer, then arms that same timer for 180 ticks.
After snapping the active destination to its route point, the callback rereads
the destination pointer for the actor-height and padding writes.

The Mummy callback at 080D453C uses the same captured motion-flag address and
the same 300-tick extension clamped to the gate duration. Both helpers are now
shared. The Mummy function still matches all 900 bytes.

The stock checker scores 0807F130 at 88.4% over the reviewed 904-byte extent,
up from 55.9%. The draft is four bytes long. The ROM spills the busy result
and tag, retains the control pointer across the response-slot call and keeps
the contact-ownership mask live across the node release.
