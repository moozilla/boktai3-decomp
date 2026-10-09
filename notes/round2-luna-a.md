# Luna A production round 2

- Branch: `codex/endurance-r2-luna-a`, based on `e2546f6`.
- Range: `08002000 <= addr < 080A0000`.
- Started 2026-10-09 02:07:45 UTC; finished 02:26:31 UTC (18m46s).
- Worker produced seven matching commits; integration accepted **five new functions, 328 emitted/progress bytes**. `08019BCC` and `08019C00` already existed at the baseline and their rewrites were excluded. Automatic loose clone ports: 2 (`0801DFB8`, `0802FE50`). Accepted manual C matches: 3 (`0802FDB8`, `0800F0F0`, `0801C1A0`). Broad clone porting initially found one port; after seeding `0802FDB8`, a second pass ported its sibling `0802FE50`.
- Full ROM builds printed `build/boktai3.gba: OK` before each matched commit batch. No ROM data pointers were added, so no shift test was needed.
- 24 exploratory manual candidate checks across 10 targets, plus 7 final per-file rechecks; clone-port checks are recorded by tool outcome but not included in those manual counts. The `0803FAC8` six-member family was excluded: the only representative match used explicit `__asm__("r4")` register bindings, which the parent rejected as forcing allocation. Those candidates remain only in ignored WIP.

Matched commits (one function each): `7ca0e82` `sub_0801DFB8`; `d8aea94` `sub_0802FDB8`; `9328a35` `sub_0802FE50`; `b3b9ba4` `sub_08019BCC`; `f3c1001` `sub_08019C00`; `ec463c1` `sub_0800F0F0`; `a7533a4` `sub_0801C1A0`.

Useful compiler notes: `0802FDB8` matches as a void function that increments the word field for its side effect; ordinary locals `one=1` and `zero=0` preserve the original store order. `08019C00` needs both source words loaded into locals before either destination store. `0800F0F0` matches the DMA setup when the scratch zero stays on the stack, the destination MMIO pointer is a volatile local loaded before the index math, and the index calculation is written as `i <<= 2; i += original; i <<= 6; i += 0x2C`. `0801C1A0` clears bit zero with `&= ~1` (agbcc emits `movs #2; rsbs`), then sets bit zero in the second field.

Unmatched targets retained in WIP: `0803FAC8` (ordinary C argument setup / clamp allocation remains unresolved; explicit register-binding candidate excluded), `0802A758` (mask and address register order), `08019BB0` and `08019BE4` (one saved pointer register differs), `08019C00` initial wrong-signature candidate was superseded by the exact source, and `0801C69C` (register ordering for offset stores). The older round-1 WIP and ignored logs were preserved.
