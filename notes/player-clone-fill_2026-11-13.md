# Player clone fill-local cleanup

Tags: #matching #actors #animation

`PlayerClone_UpdateBehavior`, 082003BC–08200A10, now matches without its volatile
fill local. Its concession-free line score rises from 99.4% to 100%, a gain of
0.6 percentage points for this function. All 1620 function bytes match; the
complete `src/player_clone.c` unit also matches all 4452 bytes.

The fill word and the existing 24-byte particle-motion configuration now belong
to one ordinary local scratch record. Both members are used; no padding, extra
storage, register pins, barriers, compiler overrides, or volatile objects were
added. Both vector-clear calls initialize `work.fillWord` before passing its
address to CpuSet. Particle setup uses the adjacent `work.particleMotion` buffer.

A standalone ordinary u32 changes the compiler's allocation order: the fill word
moves from stack offset 20 to 40, and the earlier angle vector moves from 3C to
38. This also changes register allocation later in the particle path and shortens
the function by eight bytes. Earlier single-word array, structure, and pointer
trials had not resolved it. Grouping the actual temporary storage preserves its
original placement and produces an exact match without volatile semantics.

The related `Ratatosk_UpdatePlayerOrbitAttack` at 081F7FD0 already groups a fill
word with a six-word particle configuration in a local scratch structure. That
routine uses the same particle initialization/configuration calls and supports
this representation. It was read as evidence and was not edited.

Validation used the stock compiler with an isolated function check followed by
`tools/check.py src/player_clone.c`. The updated concession scanner finds no
direct counted sites in the unit, and progress scoring reports no penalty for
`PlayerClone_UpdateBehavior`. No shared headers or checked-in progress reports
changed. The coordinator owns the combined ROM build and global progress update.
