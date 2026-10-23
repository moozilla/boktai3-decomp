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

## Session 2

Started from a clean branch with all prior commits merged and no open batch PR.

- 08078EE8: skipped after renewed boundary, condensed-listing and relative
  review. The full 5,956-byte body remains a single routine. No complete draft
  exists; the strongest matched relative still covers only 11% of its
  instruction windows. No new reconstruction was retained.
- 081AE0C0: drafted 83.2% from 82.4%, tentative Vanargand_Init. The reviewed
  target is 5,064 bytes; the candidate is 5,088. Realigned residue improves
  from 442 total / 101 structural to 426 / 76. Explicit constants before the
  default initialization loops fix their load order. A signed division helper
  call reproduces the untruncated result used by the final byte subtraction.
  Keeping position flags distinct across the script lookups, pinning health
  to sl, and materializing the variant/palette addresses before the player
  loop recover the palette spill and some ordering. All asm constraints have
  initialized inputs; no instruction bodies were added. The ordinary depin
  command leaves guarded drafts alone, so no minimal-hint claim is made.
  Pointer locals, volatile palette storage, helper palette arguments, entry
  pointer order, target-loop barriers and declaration-order variants supplied
  no further combined improvement. Compiler decision flips did not help;
  forced PRE bucket sizes only swapped a few residual stack accesses. The
  remaining work is constant-address reuse, PRE stack order, palette reload
  registers and repeated state tails. The source remains guarded and receives
  no exact credit.

The wrap-up marker appeared before starting 080D2584. Resume there; 52 later
assignment entries remain unattempted by this worker. The earlier skipped
080C9C04 and the reaction drafts remain available for a later closest-draft pass.

Session 2 summary: 0 matched, 1 draft improved, 1 skipped; exact code gain
+0.0000 percentage points. Across both sessions: 1 matched, 4 drafts improved,
2 skipped; 52 not attempted before wrap-up.

Session 2 combined validation after rebasing onto main printed
`build/boktai3.gba: OK`. The draft score was recorded over its full reviewed
extent. Report files and exact totals were not changed.

## Session 3

- 080D2584: drafted 84.1% from 74.8%, tentative
  Mummy_UpdateDisplacementRecovery. The complete reviewed target and candidate
  are both 4,852 bytes. Ported typed route, arrival and turn helpers from the
  retained Golem recovery draft. Placing the shared failure callback before
  the success tail recovered the repeated chase layout. Preserving a separate
  flags2CC pointer across the easing prefix and using the halfword motion-mask
  helper together improved the remaining lifetimes. An output-pointer helper
  retains the entry motion-flags address through both sound calls. Realigned
  residue fell from 829 / 333 to 409 / 68 total/structural. No register pins or
  asm constraints were retained. Compiler decision flips identified a CSE
  skip-block sensitivity; PRE table-size probes only changed a few residual
  stack accesses. Direct actor parameters, cached motion-flag values, forced
  wrap registers, volatile table slots and a shared pinned zero regressed.
  Remaining work concerns the flags2CC and motion-flags spills, repeated
  address reloads, and the two RNG table/wrap allocation choices. The draft
  remains guarded and receives no exact credit.

- 08099B44: drafted 58.1% from the measured 52.2% retained draft, which
  previously lacked its fuzzy annotation. Reviewed the complete 4,752-byte
  initializer through 0809ADD4; no hidden routine boundary was found. Typed
  route-point and targeting-state views recover address and load order. The
  optional route fallback is now read after checking Script_GetPc, matching
  the observed evaluation order. Typed attack arrays, an early-return attack
  helper and a shared clear word improve the script prefix. A callback
  installer takes the seven actor callbacks as arguments, recovering their
  preload order; the collider flag clear uses the observed collider field.
  Realigned residue improves from 1,413 / 881 to 1,253 / 790 total/structural.
  The candidate remains shorter than the target; missing address-hoisting
  expansions, callback setup, animation allocation and PRE stack layout still
  dominate. Flattening either large helper regressed. Compiler flips show
  CSE skip-block and follow-jump sensitivities but no near-exact solution.
  No register hints were added. The complete guarded score was recorded.
