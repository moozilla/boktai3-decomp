# Scene input navigation

The reviewed E7D8–EA58 cohort contains ten exact functions /640 bytes. Four
new matches add 340 bytes (+0.0141 percentage points); six existing leaves
receive typed contracts. The key-repeat helper retains one minimized empty
register barrier. The other nine functions use ordinary C.

The navigation view describes the same storage as scene_input_grid.h. Its
physical axes follow E9A0's directional-key dispatch: cursor X at byte1 ranges
0–8 and cursor Y at byte2 ranges0–3. The 36 signed map cells begin at C,
column-major. Byte3 holds the selected value; bytes4/5/6 save X/Y/selection.
Byte6 also supplies a fallback while rebuilding the grid. Older row/column
names in the builder view are provisional; they do not establish physical axes.

Up/down wrap among four rows, left/right among nine columns. A candidate must
be nonnegative and differ from the current map value. A successful movement
updates the cursor and selection, plays song DC, and returns1; exhausting the
axis returns0. The dispatcher checks vertical bits C0 before horizontal30.
Opposing directions cancel their axis. If vertical bits are both set, its
default branch still considers horizontal input.

Fresh directional presses reset the repeat byte and return the full pressed
halfword. Otherwise held directional input increments the byte, returning the
full held halfword when the counter exceeds9 and resetting the counter.
Without held directional input the counter clears. The counter's byte wrap
and volatile fresh input reads are preserved.

E9EC positions a widget through E394 using the current selection. EA00/EA10
save/restore cursor and selection. EA20 narrows kind and initial coordinates,
sets the full-word flags argument through E7C4, reads the initial selection
through E5D0, and clears repeat. These contracts establish reusable scene input
behavior; no specific screen or new runtime coverage is claimed.
