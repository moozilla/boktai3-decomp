# Saved-position marker, 08053D8C–08054294

All nine functions match across1,288 contiguous bytes. Three new bodies add0.0439
percentage points (1,060 bytes):172 without hints and888 with minimized
concessions (idle3, initializer22). Existing predicate/state/dormant/tick/
destructor/creator helpers were typed, named and checked over full extents.

This list9 task is0xCC bytes: nearby18, variant1A, primary sprite1C/graphics48,
origin64, secondary sprite6C/graphics98, palette baseB4 (legacy field name baseTile), activationFrameB6,
display heightB8, input cooldownBA, two halfword script argumentsBC/BE,
integer script IDC0, stateFrameC4 and callbackC8.

The proximity helper reads **GameState.positionX/Z at02000710+30/+34**, not the
live player actor at02000580. It applies the96-unit box and squared radius9216;
no player busy/form gates occur here. The stored-position interpretation follows
the established GameState header and remains provisional as to runtime timing.

Idle uses the same cooldown/input/status gates and player interaction0815B0A8
as the preceding marker. Display height moves by twos between0 and24; activation
moves between0 and8. Activated graphics select palette168, while all partial/inactive
states use paletteBase. Interaction lowers the primary, waits for live player action0,
clears primary flag200, sets priority3, hides the secondary and invokes an optional
script with two expanded halfword arguments, then remains dormant.

Script t selects variant. Position p is terrain-adjusted using the shared
surface1/2 slope formulas; R gives the script ID, and A gives two arguments.
Variant0 selects palette166 and nonzero selects palette167. Both sprites use resourceA152,
priority3, flags200/400 and copied origin. No optional body is allocated. Creator
forwards two unused script-call arguments as in the original. The separate
SavedPositionMarker name avoids confusing this predicate with the live-player
predicate of InteractionMarker. No new runtime coverage is claimed.

Palette evidence: new display-track update54EC0 interpolates sixteen RGB15
colors from the32-byte banks of030042E4. The established helper name
AnimData_SelectTiles is therefore misleading; it selects palette banks.
The source field baseTile in these marker views is a retained legacy name,
with semantic renaming scheduled for the next combined build.
