# Interaction marker, 080537D0–08053D8C

All nine functions match across1,468 contiguous bytes. Four new bodies add0.0542
percentage points (1,308 bytes):316 without hints,992 with minimized concessions.
The600-byte initializer retains28 minimized hints/barriers, the392-byte idle
update three. Existing state/dormant/tick/destructor/creator helpers were typed,
named and checked over full extents.

Task allocation118 on list9: id18, nearby1A, bodyEnabled1B, optional body20..68,
primary sprite68/graphics94, originB0, secondary spriteB8/graphicsE4, palette base100 (legacy field name baseTile),
activationFrame102, display height104, input cooldown106, two halfword arguments
108/10A, integer script ID10C, stateFrame110 and update114.

The nearby predicate rejects GameState.form3..5 or player busy464. Otherwise it
requires the local player's X/Z position to lie within96 units on each axis and
within squared radius9216. This helper returns a conventional0/1, unlike the
preceding proximity-emitter helper's nonvoid fallthrough.

Idle increases input cooldown to10 unless engine flag200 is set. Enter/leave
proximity plays198/199. Activation progresses to8 and display height approaches24
by twos; leaving reverses both. Primary is hidden at height0, otherwise follows
originY+height. Palette selection advances the base by0/1/2 according to activation.
A press requires both input-enable02000560 bit1 and newly-pressed03005260[1] bit1,
player status without100000, and cooldown10. It notifies08056C6C(1), invokes local
player interaction0815B0A8(player,position,0,4) and enters the interaction callback.

Interaction lowers the display two units per tick. At height0 and player action
457 equal0, it clears primary flag200, sets render priority3, hides secondary,
and enters a dormant callback. An optional Script_Call receives two u32 values
expanded from the stored halfwords. The argument descriptor's unused upper
halfword is retained as in the original; only its count and pointer are consumed.

Script initialization stores the creator id, reads p XYZ and replaces Y from
terrain height (surface1 subtracts low Z; surface2 low X), reads script s and two
arguments A, and uses h to select the optional body initialized by08225AB4 and
08003C00. The two sprites use resourceBA8, frame7, priority3 and paletteA9, with
flags200/400. It installs the idle callback. Body/destructor APIs and exact
in-game marker identity remain tentative. No new runtime coverage is claimed.

Palette evidence: display-track update54EC0 interpolates sixteen RGB15 colors
from the 32-byte banks of030042E4. The helper is now named
AnimData_SelectPaletteBank (formerly AnimData_SelectTiles), and the marker fields
are named paletteBase. These names replace the earlier tile interpretation.
