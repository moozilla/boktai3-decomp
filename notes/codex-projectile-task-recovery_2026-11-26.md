# Timed projectile and trail source recovery

Tags: #effects #types #matching #phase3

The projectile family went from 42 to 35 diagnostic lines and 45 to
39 constant uses. Every changed unit matches stock agbcc.

The projectile manager now names the next trail slot and the four
trails' angle offset, radius, and timer. The trail spawn at 081D76F4
uses the actual projectile direction and position. Its byte
conversion replaces an assignment to a placeholder local. The clear
routine at 081D7874 matches an indexed loop through the four entries
with literal reset values. The update at 081D75E8 uses the real owner
and projectile layouts with its existing position-first cursor. The
straightforward indexed motion loop changes induction and shortens
the routine by 52 bytes.

The lifetime and playback callbacks read the shared projectile
fields from the real frame, parameters, and sprite. Arc flight saves
and tests its signed starting height at 0x17E. The two literal frame
29 stores in the terminal animation code match through the existing
sprite pointer. The default animation's local frame still
materializes at a different point when written directly.

The timed-effect contact setup at 081DA048 takes the opaque owner
used by the collider callback ABI. Converting that formal to a local
projectile pointer enables a direct flag store. Using the projectile
pointer directly as the formal changes its allocation and the
availability of the contact-test value. The timed initializer uses
the same 0x1AC-byte projectile layout as the allocator. Its CpuSet
stack word is a fixed hardware fill source read by address.

The homing scan at 081D905C retains the union after separate pointer
variables and direct collider positions changed the stack and
reload choices. A diagnostic spill rotation reduces part of the
difference and the natural variants still differ. Several small
timed callbacks also construct the render mask before reading the
flags. Their literal variants retain the length but change
instruction order.
