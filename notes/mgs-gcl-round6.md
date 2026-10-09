# MGS/GCL matching round 6

Sol 6.1 worker on `codex/mgs-gcl-round6` from `6faa70e`.
Started 2026-10-09 03:54:15 UTC; completed within the fifteen-active-minute bound.
Final result: **7 new matches / 1,236 native bytes**.

Expanded adjacency inventory checked private and main C coverage. Parent reserved
ten addresses before edits: BA5C, E040, E190, E3C0, E52C, E5C4, E860, E8C8,
F15C, 210B8 (all with 0821 prefix except 082210B8). Previous three misses
were not repeated. All comparisons, widths, limits, negative variants and
source differences are documented in `docs/MGS_GCL_MATCHING.md`.

No upstream C copied/adapted. Strong source correspondence established BA5C's
expression evaluator role; B3 uses inline low-five-bit operators and has
procedure-argument assignment and block-valued operands absent from MGS Expr.
Safe integer slot-address arithmetic avoids pre-array C pointer arithmetic.
210B8 connects existing 19C00 to table generation and verifies the earlier
123456 state write as a PRNG seed. Seven byte-exact independent sources
were installed and full tools/build.py printed `build/boktai3.gba: OK`
before these one-TU commits:

| Commit | Function | Native bytes |
| --- | --- | ---: |
| `c45c269` | `0821BA5C` | 160 |
| `112a17c` | `0821E040` | 144 |
| `3e9d720` | `0821E190` | 360 |
| `0d21243` | `0821E3C0` | 276 |
| `6bf81af` | `0821E52C` | 104 |
| `f883a06` | `0821E860` | 104 |
| `c3584fd` | `082210B8` | 88 |

Unmatched E5C4/E8C8/F15C valid drafts remain ignored WIP. E5C4 permuter
failed its duplicate-AST-node assertion without saving a candidate. 210B8
60s search yielded an audited ordinary do scope, then independently checked.
F15C 60s search yielded byte-exact identical branch duplication, which is
behaviorally safe but artificial and not promoted. No unsafe candidate or
nonmatching C tracked. Parent handles combined regression screenshots.
No make/shared-gen/push/PR/screens/new agents or speculative names.
