# Level advancement display (tentative presentation identity)

`08056F20–0805751C` owns singleton `gUnk_02000120`, a priority-9 task of size
`0x260`. The next function begins a separate countdown manager. The static
contracts support the name `LevelAdvanceDisplay`; no execution coverage or
particular on-screen artwork is established here.

`include/level_advance_display.h` gives a shared layout. The sprite is at
`0x18`, graphics at `0x44`, animation kind/active/frame at `0x60/0x62/0x64`,
secondary-event kind at `0x72`, next threshold at `0x74`, threshold-table
pointer at `0x78`, and particle resource at `0x7C`. Eight `0x3C` particles
begin at `0x80`. The main sprite's frame is at `0x28` and position at `0x34`;
a particle's position is at `0x18`, local XYZ offsets at `0x28`, velocities at
`0x30`, active halfword at `0x38`, and frame at `0x3A`.

The script initializer optionally reads `e` with the safe value reader as a
table pointer. Threshold lookup returns signed `table[level] * 10`. At levels
through 98, initialization caches the current threshold. Each manager update
checks whether the unsigned word at game-state `0x48` reaches that threshold.
When it does, level at `0x40` increments once, halfword `0x42` increases by two,
and the next threshold is read using the new level. The experience and bonus
interpretations of those two fields are tentative; the existing shared game
state already names the signed level field. Pointer reloads between writes
are preserved with empty memory barriers where the typed view would otherwise
cache them.

The main sprite uses resource `0x1C1D`, palette bank 1, priority byte 1, and
starts hidden. Particles use resource `0x1C1E`, initial frame zero, scale
`(-4, -4)`, and helper mode 1. Normal advancement starts animation kind 1,
shows the main sprite at player position plus 250 Y, clears its frame counter,
and resets all particle animation frames to zero. It plays sound 904. A second
entry starts kind 2, stores the secondary-event kind, selects sprite frame 3,
and plays sound 897. Its event poll currently always returns -1; its manager
argument is still part of the observed call contract.

During kind 1, active particles update first. Before incrementing the manager
frame, frame values 0, 4, ..., 60 launch a particle at slot `(frame >> 2) & 7`.
Launch clears the hidden flag, picks offsets X/Z in -63..64 and Y in 64..127,
and sets upward velocity to 12..19. Its local frame starts at zero. Motion
updates frames 1..15, add upward velocity, re-anchor positions to the player,
and alternate animation frames 2 and 3 according to frame bit 2. At frame 16
the particle hides and deactivates. The random-index stores occur after the
third and fourth launch samples, and after the third shake sample; their
ordering is preserved.

Both main animation kinds copy the player's current position each update.
For frames 1..4 they add `250 + 30 * frame` to Y. Later frames add 400; frames
5..19 also add independent XYZ offsets in -15..16. Kind 1 shows frame 0
through tick 119, frame 1 at 120..122, then frame 2 at 123..125. Kind 2 uses
frames 3, 4, and 2 over the same intervals. At tick 126 the sprite hides and
kind/active clear. The secondary-event value at `0x72` is not consumed in this
reviewed range.

If the player's word at `0x1C` equals 4 and manager active equals 1, update
hides the main sprite and every particle and clears their active fields. It
preserves animation kind. Otherwise it advances the animation, checks for a
new level, and polls the disabled secondary event only when kind is zero.
Destruction removes the main sprite and all particles, clears the singleton,
and returns zero. Creation reuses an existing singleton and preserves the
task initialization failure branch.

All fourteen functions match their complete reviewed extents. Ten are newly
recovered, totaling 1,376 bytes, a 0.0570 percentage-point gain; four previously
matching functions totaling 156 bytes now use the shared contracts. Empty
barriers and remaining register bindings are temporary matching concessions,
minimized before commit; no explicit r7 binding or handwritten instruction body
is used. Combined ROM validation belongs to the integrator's batch.
