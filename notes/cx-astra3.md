# Campaign worker astra3

Tags: #matching #nonmatching #golem #scene-actors

- **080C12FC — drafted 82.0%** (previous 80.2%, +1.8 percentage points).
  Localized chase helpers so this function can be refined without changing other
  drafts. Moved the invalid-player callback to the shared tail, retained the
  materialized byte boolean, borrowed the counted turn-selection loop from
  080C09F0, and corrected the hit path's nonexistent context timer reset.
  Realigned differences improved from 739 total / 315 structural to 600 / 255.
  Remaining differences include helper pointer lifetimes, flag-mask ordering,
  turn-loop allocation, and stack layout. No exact credit.
- **08078EE8 — skipped after partial reconstruction.** Confirmed the complete
  5,956-byte boundary and inspected the condensed listing and matched relatives.
  No available Ghidra export or committed full draft; strongest relative covers
  11% of the instruction windows. Scratch reconstruction through 08079160 covers
  timer decay, availability, collision gating and effect selection, compiling to
  600 bytes. It is incomplete and is deliberately excluded from production.
- **08099B44 — drafted 52.2%** from no production draft. Reconstructed the
  complete Ghost initializer: drawable/resource setup, script options, path
  placement, palette decisions, callbacks, animation stepping, and registration.
  Splitting script and callback setup into inline helpers reduced structural
  differences from 1,152 to 881. Final realigned differences: 1,413 total / 881
  structural. Field-address hoisting and helper scopes dominate the residue.
  Verified directional-frame outputs are assigned on both callee branches.
  The repair tool is unavailable because the instrumented compiler is missing;
  the ordinary per-function compiler works. No register hints or exact credit.
- **080CCD7C — drafted 81.9%** (previous 80.4%, +1.5 percentage points).
  Ten helper/width/zero-lifetime variants found one useful change: materialize
  the turn block's word zero before saving the new heading. The retained empty
  operand barrier is marked as a register-pinning concession. Realigned residue
  improved from 539 total / 141 structural to 501 / 139; the candidate now has
  the target's 4,504-byte size, but remains nonmatching. Explicit r4/r5 pins,
  a second byte-zero barrier, RNG spelling and flag-clear helpers regressed.
  Depin skipped guarded hints; no minimal-hint or exact-match claim.
- **0817C3F4 — skipped, existing 83.3% retained.** Eight variants covered
  initialized event-pointer scopes, distinct actor-copy lifetime, helper widths,
  callback early return, explicit inner exits, and a state subview. None improved
  the baseline 530 total / 407 structural differences. Kept production unchanged;
  no uninitialized event-pointer candidate was accepted.
