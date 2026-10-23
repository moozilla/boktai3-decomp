# Campaign worker xhigh

Tags: #matching #monsters #compiler #register-pinning

## First batch

- 080C9C04: skipped after reviewing the entry, condensed repeated movement
  paths, current relatives and prior recovery notes. The complete extent is
  8,764 bytes; no separate routine boundary was identified. Matched relatives
  explain terrain and route fragments, but not the four surrounding recovery
  paths. No complete candidate or progress claim retained.

- 080D812C: matched Mummy_UpdateBurnDamage, 6,504 bytes in the Register-pinned
  category (+0.2698 percentage points of code). Starting draft was 99.9%, with
  only the 080D8390 signed-load scratch differing (r5 instead of r4).
  hint --flips confirmed allocation, and rasolve found an exact compiler-space
  solution by shifting reload 17 forward once (or reload 18 backward once).
  Local-player width, offset pins, RAM-literal forms, intensity-pointer and
  count lifetimes, empty loop boundaries and ordinary near-match depinning
  did not close it. The allocation-only token pattern documented in
  docs/ASM_OUTPUT_QC.md provided an exact bridge by reserving r5/r2/r3 around
  the signed load. Ordinary depin then removed a defining output while leaving
  its input; that intermediate result was rejected. A safe second pass
  preserved the definition while removing other hints. After reaching a new
  exact hint subset, both token ends and all token variables became removable.
  The final source retains four register pins, no allocation-only tokens and
  no empty asm constraints. The complete reviewed 6,504-byte check is exact.

- 080C12FC: drafted 84.7% (stored baseline 80.2%; measured baseline 82.0%).
  Pinned the prior turn direction to r7 and the RNG mask to r1, and expressed
  the four attempts as a retry loop with one chosen-result tail. Realigned
  residue improves from 600 total / 255 structural to 522 / 240. Candidate
  length remains 6,320 bytes against the complete 6,372-byte target. A common
  motion-flags pointer plus inverted retry polarity reaches 503 / 223 but
  scores 84.6%; retained as a scratch alternative, not the production draft.
  Turn argument permutations, mask widths, table lifetime and zero placement
  did not provide a further consistent improvement. Remaining work is mainly
  repeated arrival helper lifetimes and PRE stack layout. Recorded the score
  using the emitted address label, then restored the promoted fallback name:
  its __addr symbol is required by the combined linker script. The initial
  combined build caught that fallback-label mismatch before integration.

Batch validation after rebasing onto main: the final exact Mummy source uses
four register pins and no empty asm constraints; the combined campaign build
printed `build/boktai3.gba: OK`. The Golem draft remains guarded.

## Continuation

- 080DA97C: drafted 86.5% from 86.3%. An explicit signed mask local delays
  the entry word load to the original position. A shared zero passed through
  the arrival control and behavior helpers, plus the scoped signed group
  mask, reduces realigned residue from 448 / 135 to 419 / 127 total/structural.
  No new register hints. Whole reviewed extent is 6,316 bytes; candidate is
  6,272. Shared motion-pointer variants, direct masks, cached entry flags and
  separately constrained turn-group lifetimes regressed. Compiler flips and
  automatic repair supplied no improvement. The guarded final score is
  recorded; remaining issues are PRE stack ordering and arrival/turn layout.

- 0808B3EC: drafted 81.2% from 79.2%, tentative
  Ghoul_ResumeBehaviorAfterHit. The prior turn direction in r7 and RNG mask
  in r1 reduce the turn expansion; a shared arrival zero and explicit signed
  group/context masks recover store/load ordering. Realigned residue improves
  from 818 / 394 to 693 / 356 total/structural. Candidate length is 6,204 bytes
  against the 6,236-byte target. Widened motion flags score 81.3% but worsen
  both realigned measures, so were rejected. Explicit retry-path nesting was
  byte-equivalent; table lifetime and other prior-register choices regressed.
  Two pins are marked as a concession; the normal build keeps the assembly.

## Wrap-up

The stop marker appeared before starting 08078EE8. No later function was
started. Remaining 54 assignment entries are deferred, beginning at 08078EE8;
this is distinct from the one attempted-and-skipped Golem function.

Summary: 1 matched, 3 drafts improved, 1 skipped; 54 not attempted before wrap-up.

Wrap-up batch validation after rebasing onto main: both guarded draft scores
were recorded over their complete reviewed targets, and the combined build
printed `build/boktai3.gba: OK`. No exact progress is claimed for the drafts.
