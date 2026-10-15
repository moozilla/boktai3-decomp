# Script-controlled display tracks

The complete cohort is `08054B80–08055678`: fifteen functions / 2,808 bytes,
owned by manager `gUnk_02000494`. Eight newly exact functions contribute 2,532
bytes / +0.1048 percentage points; seven existing helpers (276 bytes) receive
no new credit. Identity is provisional and based on static consumers. There
are no new runtime coverage claims.

`include/display_track.h` records dynamic capacity/count at manager18/19 and
0xB8 entries at1C. The creator uses list9 and allocates1C+capacity*B8, with script
m capacity default1. Each entry has a tag0, independent track flags2, mirror3,
run-during-pause byte4, sixteen blended RGB15 colors8, sprite28, graphics54,
origin70, target78, movement duration/frame80/82, palette base/start/end/frame/
duration84..8C, scale start/end/frame/duration90..96, sequence id/row/event98..9A,
sequence state9C, sequence resourceAC, completion script IDB0 and owned shadowB4.

The tag lookup requires n and compares the full script value with a halfword
tag; it reloads the singleton after parsing. Other controls set sprite frame
or priority with p, set/clear hidden flag1, or install movement/palette/scale/
sequence tracks. Movement reads p XYZ and f duration, captures the current
sprite position as origin and resets frame when duration is nonzero. A zero
duration does not clear an already active movement flag. Each tick increments
frame, computes three signed weighted Div interpolations, and clears flag1 at
completion. This flag is distinct from the sprite's hidden flag.

Palette control reads s/e/f. Durations0/1 immediately select base+end; longer
tracks reset frame and set entry flag2. Update blends all sixteen RGB15 colors
from banks base+start and base+end, weighting each 5-bit channel separately by
remaining/current frame and dividing by duration. It publishes the private
palette buffer through graphics+C and sets graphics palette index to600+tag.
At completion it selects the final shared bank and clears the track flag.
This consumer proves the former AnimData_SelectTiles label was wrong; the
shared API is now AnimData_SelectPaletteBank. Its keyframe counterpart is
Anim_SelectPaletteBank, and the wrapper is DisplayAnimation_SelectPaletteBank.

Scale control also reads s/e/f. The end value is initially copied into both
sprite scale bytes; signed byte64 represents identity. The immediate setter
sets sprite flag2 when nonidentity but does not clear an existing flag at64.
The running updater interpolates scale, and clears sprite flag2 when its final
signed byte is64. Entry flag4 tracks this independent operation.

Sequence control requires m id, d row and r mirror; E chooses the alternate
initializer08220D20, and P adjusts the halfword playback speed via08220D44.
The shared DisplaySequenceState fields match observed commands, mirror/loop/
reverse flags, frame count, frame weight, duration, index, speed and elapsed.
Each command's high bits select frame, bits4/5 combine with sequence mirror
flags, and the low nibble weights duration: signed(speed*weight)>>6, minimum1.
Both update and initialization display the current frame before advancing the
elapsed/index state. Initialization therefore consumes a playback tick.
A nonzero o stops sequence tracking on completion; optional e supplies the
integer script resource ID called with argument index0. Completion with o=0
continues the sequence behavior.

Entry initialization requires t graphics resource; optional n tag/i frame/c
palette/f mirror/p XYZ/A run-during-pause/R priority configure the sprite.
Optional m looks up a sequence resource under extension key922E and reads id
and row, then o/e/P. Optional s allocates a64-byte shadow, clears it and binds
it to the sprite's position. Count increments only after successful required
parsing. The manager skips entries during global pause unless their A byte is
nonzero; destruction removes sprites and any owned shadow before clearing the
singleton. Shadow teardown08013684's void helper contract remains provisional.
