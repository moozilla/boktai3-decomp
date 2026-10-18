# Foundry display resource functions, 2026-10-18

Tags: #matching #assets

Candidates remain in `wip/foundry/`; per-function results are in
`build/foundry/sub_XXXXXXXX/result.txt`. No source promotion or commits.

Exact foundry checks: 08213010 (one check, ten-entry value lookup), 08214588
(two checks, reset display sequence sprite resource), 08215148 (two checks,
DMA3 copy of resource tiles into OBJ VRAM), 08215490 (four checks, reset OAM
queue and optionally disable all 128 entries), 08216520 (two checks, convert
pixel position to background tilemap load coordinates), and 08217298 (three
checks, reset a two-slot resource cache). Names are provisional, confidence low.

08216E54 remains a 73.7% draft after two checks: recognizes uncompressed map
signatures and otherwise decompresses a keyed resource into a cached buffer.
The original additionally computes the returned address through unresolved
arithmetic involving literals 0401FB14 and FDFE0C00; the simpler draft does not
reproduce this. Its compressed-buffer arithmetic needs investigation before
acceptance; the fuzzy score uses candidate length.

08217DE0 remains a 90.9% draft after six checks. Its CORE distance is zero,
with four differing halfwords in the register allocation of the queue selector.
Explicit unsigned negation and right shift reproduce the desired operations;
boolean comparisons instead produce a shorter shift-and-mask sequence.

Shared layouts were extended only within padding: DisplaySequenceSprite in
`include/display_animation.h` now exposes the resource pointer, playback bytes,
and packed attributes. LevelAdvanceParticle in `include/level_advance_display.h`
now exposes queued state, scale, rotation, palette slot, mode, status, frame
state and intrusive links. `foundry.py hdrcheck` passed all 16 and 14 matched
consumers respectively. Names of currently unconsumed bytes are tentative.
