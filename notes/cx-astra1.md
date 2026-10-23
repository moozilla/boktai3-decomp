# Campaign worker astra1

Tags: #matching #monsters #register-pinning

- 080C9C04: skipped for a later revisit. Reviewed full condensed control flow,
  the first arrival path and matched terrain/charge relatives. Four repeated
  arrival/turn paths dominate the 3860 instructions; partial helper coverage
  does not establish the surrounding recovery/charge transitions. No complete
  candidate or exact progress claimed.
- 080DA97C: drafted 86.3% (from 85.4%). Pinning the prior turn direction to r7
  reduces realigned residue from 459/147 to 448/135 (total/structural).
  Tested flag-mask widths, route success polarity, turn-loop scope, RNG-table
  lifetime, prior widths and retry/RNG register choices. Retained only the
  improvement in both structural and total differences; a table local reduced
  structure slightly but increased total differences. Remaining issues are
  PRE pointer/stack lifetimes, turn-loop scheduling and far-branch placement.
- 081AE0C0: drafted 82.4% from no draft; tentative Vanargand_Init. Recovered
  script options, sprite/part construction, palette interpolation and all four
  starting modes. The matched 081ACC24 palette helper covers five repeated
  sections. Reviewed 5064-byte extent; candidate 5072 bytes, 442/101 remaining.
  Explicit packed position bitfields recover the stack input format. Constant
  lifetimes and palette/state-tail allocation remain.
- 080C3260: skipped after reconstructing the initializer stages against the
  condensed listing and current relatives. Largest relative covers 14%;
  drawable variants and extensive inline script-option handling remain without
  a complete candidate. Revisit after a related initializer is recovered.
- 080E25FC: drafted 84.6% (from 83.5%). Failure-first reroute plus prior direction
  in r7 improves 386/148 to 356/124. Width, flags-pointer, RNG-table, retry pin
  and choice-byte variants did not improve both metrics.
- 080B43BC: skipped. Rechecked the 1861-instruction Vorn initializer and the
  earlier partial-recovery record. Its large PRE block hoists dozens of
  callback/group-field addresses before script parsing; the best exact relative
  explains 11%. A prefix-only port would not establish full-function progress.
- 080FE68C: skipped. Compared drawable creation and attack/script setup against
  the condensed body and exact relatives. Generic terrain/animation code covers
  15%; callback-address hoisting and full option records still lack a C base.
- 080DCC98: skipped. Reviewed context/category setup, both drawable paths,
  collider/effect initialization and terminal control installation against the
  prior recovery notes. Same unresolved large initializer/PRE layout; no new
  complete candidate retained.
- 080BBDD0: drafted 89.8% (from 88.9%), 218/22 realigned differences versus
  256/46. RNG inline plus table local recovers halfword sampling and removes
  loop spill differences; control argument order and direct player-slot return
  remove copies. One empty mask constraint improves state-flag scheduling.
  Reviewed 4116-byte target; candidate 4104 bytes. Pinning loop locals did not
  help. Remaining PRE stack slots and masks/copies still prevent exactness.
- 080F0C20: skipped. Identified Ivy initializer: context field 62C, category 24,
  animation resource D638/sequence 99DD, then the same extensive actor option,
  route, terrain and callback setup seen in the preceding initializers. No
  complete candidate; shared generic helpers alone do not reconstruct it.
- 081ABA64: skipped. Reconstructed phase boundaries and reviewed the six packed
  playback expansions against 081B5A98. The 11-phase attack mixes debug input,
  track motion, a stack-packed scale pair and synthetic contact. Interior
  far-branch labels are shared tails, not callees. No complete improved C.
- 08090FEC: skipped. Checked Slime setup (field40A, category22, B96C/A929
  resources) and later parameter/collider/control stages against the earlier
  full recovery record. Prior full draft was 43.8% and absent from production;
  current inspection did not establish a better whole-function shape.
- 080ABDCC: skipped. Tentacle initializer confirmed by category9, profile and
  segment setup; resources D636/C102. Reviewed the segment-history, palette,
  collider, callback and initial route tail. Extensive shared actor setup still
  needs a full initializer reconstruction, beyond the available exact helpers.
- 0814C3A4: skipped. Reviewed entry snapshot/reset logic and phase calls against
  Player_OnGrabMotionExit and the earlier full control-flow notes. Grab-motion
  phases combine trig, particles, temporary animations and actor sequences;
  current relatives do not provide a complete base. No draft promoted.
- 0801F93C: skipped after a complete experimental controller reconstruction.
  Recovered message commands, 17-type dispatch, player/tether proximity lists,
  conditional interaction scripts and slot callbacks. Baseline 39.1%; manager
  r8 and boolean-tail variants reached about 44%, still hundreds of structural
  differences. Main blockers are packed-vector lifetime, duplicate case layout
  and unused initial player-position evaluation. Scratch only; not a close draft.

- `sub_08087018`: skipped after complete scratch reconstruction (62.1%; 587 total / 201 structural instructions differ). Reused the Mummy arrival helper and recovered both delayed recovery paths. Explicit actor pins improved the fuzzy score but worsened structural alignment, so were rejected. Main allocation and PRE spill lifetimes remain off.

- `sub_080BFCC4`: drafted 95.5% (stored baseline 94.4%; current compiler baseline 94.6%). Full-width random-turn return plus a constrained prior-direction local reduced 59 total / 15 structural differences to 45 / 10. Register-pinning concession recorded. Remaining: loop constants/current-value allocation, PRE spill ordering, and destination offset materialization. Tested helper widths, argument orders, declaration swaps, loop variants, and masks; further variants stopped improving.

- `0813EDDC` / `PlayerAction_RefreshFlagState`: matched, Register-pinned category (2,412 bytes; +0.100 percentage points of code). Recovered all equipment descriptor cases, bonuses, resistance transformations and internal finalization tail. Explicit unused switch cases preserve the original jump-table extent. Separate clear-loop index and explicit party-slot address arithmetic preserve allocation and instruction order. Typed `savedSpriteResource` removed a redundant sentinel copy. Four local register pins and three empty constraints close descriptor temporary allocation; depinning removed one unnecessary constraint. The caller now declares and passes the existing third argument; its bytes remain exact. Full `src/player_3.c` fast check: zero differing bytes across 13,128 bytes.

- `080D7568`: drafted 85.0%, tentative `Mummy_ProcessContactPursuit`. Full contact selection, heading/distance update, arrival routing, direct pursuit, and delayed restoration reconstructed. Context-mask helper, explicit random retry loop, embedded contact-state view, full-width main motion mask, and prior-direction pin reduced the initial 329 / 84 realigned differences to 196 / 64. Candidate and reviewed ROM extent both 2,360 bytes. Remaining: turn allocation, PRE spills, and flag-address lifetimes.
