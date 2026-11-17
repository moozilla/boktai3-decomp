# Round 14: shared random-table draw operation

Tags: #round14 #matching #once-loop #random-table

Date: 2026-11-16. Source baseline: `36880fd91` on `codex/round14-fields`.

The recovered operation advances a cursor through the 1,024-entry random table
and captures the selected halfword. `PARTICLE_RANDOM_DRAW` contains those two
operations and is shared by ContactBurst_Update and the presentation shake
sampler. It adds no temporary, pointer copy, dummy use, or allocation parameter.

## Source evidence

* ContactBurst_Update already performs the increment, mask, cursor store, and
  halfword read together. Its eight-particle burst uses the sample's low four
  bits to perturb each particle's direction. The cursor is reached through the
  existing `randomState` pointer.
* Presentation_UpdateDropShakeBlink performs the same increment and read in
  ShakeSample, using `gRandomIndex` directly. Eight calls across four animation
  phases use the samples for horizontal and vertical jitter. The old expansion
  puts the increment in a one-shot loop and the read in a separate single-store
  macro.
* The existing ParticleRandom_NextSample and ParticleRandom_ReadDirectSample
  inlines in `monster_particle_random.h` independently identify advance-and-read
  as one reusable operation. The former also accepts a cursor pointer and a mask;
  the recovered macro covers the fixed 1,024-entry case used at both sites.

The original macro spelling is unknown. The evidence is the repeated complete
operation and its existing sampler API. This change makes no source-backed
ignore claim and adds no `concession-ok` comment.

## Natural alternatives and limits

Plain loop removal and the existing direct sampler were tried first.
ContactBurst_Update remains 392 bytes but swaps the table and collider addresses
between r8 and r9. Its full 1,080-byte unit differs by five bytes. The existing
pointer-and-mask sampler instead produces a 1,092-byte unit. Removing the
collider's redundant reader/writer locals or using its field directly does not
restore the match.

Plain removal of ShakeSample's increment loop leaves the 576-byte unit with 24
changed bytes. Replacing the whole helper with the existing direct sampler
produces 556 bytes. The shared complete draw matches all 576 bytes.

TimeResult_UpdateDigitReveal is another user of the same conceptual operation,
but its current source remains open. A direct shared sampler changes four bytes
in its 2,056-byte unit, swapping its graphics-resource and table addresses between
r8 and r9. Indexed digit traversal and direct field arguments do not match. The
new complete-draw macro changes ten bytes there, so it is not adopted at that
site.

## Validation and accounting

* Both changed source units independently match stock agbcc: 1,080 bytes for
  `contact_burst.c` and 576 bytes for `fn/sub_081E2910.c`.
* Every source unit including `monster_particle_random.h`, directly or
  transitively, independently matches: **84 of 84**.
* The compiler-inclusive scan of the header and both sources falls from four
  warnings to two: two reported once-loop sites are removed. ContactBurst_OnContact's
  separate one-shot loop and the presentation function's local union remain
  visible. There are no errors or new warnings.
* The shared macro has two source call sites. Its header addition is committed
  separately; the two caller replacements form one commit so no intermediate
  source commit introduces a single-use statement macro.

Runtime drafts, independent checks, compiler diffs, and the dependent-unit check
are under `build/round14-fields/once/`. The accepted header, caller changes, and
this note preserve the source decision; the unmatched alternatives are not
installed in production source.
