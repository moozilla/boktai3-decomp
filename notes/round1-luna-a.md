# Luna A production round

- Branch: `codex/production-luna-a`, based on `540daab`.
- Range: `08002000 <= addr < 080A0000`.
- Started 2026-10-09 01:01:45 UTC; completed 01:13:26 UTC (11m41s).
- 15 verified matches, 960 emitted function bytes, across 22 distinct checked targets and 42 candidate checks. Whole-ROM builds printed `build/boktai3.gba: OK` after each commit batch.
- Clone porting was attempted once over the assigned range and ported 0 functions. No candidate was propagated from a clone family; the repeated allocator-wrapper pattern below was written from observed siblings.

Matched commits (one function each): `bea1228` `sub_08002CB0`; `fce40f5` `sub_0800A7D0`; `3266a0b` `sub_0800A78C`; `78191fe` `sub_0800CB60`; `5a147cd` `sub_0800F80C`; `ad324c8` `sub_0800F474`; `b970528` `sub_0800E390`; `a52e377` `sub_080111A8`; `451742a` `sub_080114CC`; `c05f69a` `sub_08013380`; `eb185a1` `sub_08018EF0`; `9343933` `sub_0802091C`; `9e7af38` `sub_0801B454`; `454a61a` `sub_0801D0BC`; `bbbb320` `sub_08037D80`.

The strongest reusable pattern is the allocator / callback registration / initializer wrapper: save the original argument(s) in callee-saved registers, allocate, register two callback function pointers, call the initializer, free and return zero on a negative result, otherwise return the allocation. Explicitly declaring the initializer's signed return and the argument width reproduces the failure branch and any narrowing shifts. Many siblings differ only in allocation type/size, callbacks and initializer signature.

Failed candidates remain in `wip/`: `sub_080031AC` (large offset and register lifetime), `sub_08004018` (loop-entry layout and multiplication/register order), `sub_080096A0` (initial pointer/mask register order), `sub_08011AA4` (final absolute store register order), `sub_08012D3C` (destination struct/register allocation), `sub_08012F28` (final `ldrh`/`movs #4` operand order), and `sub_0802D570` (allocator result lifetime/branch layout). The closest near-match is `sub_08012F28.c`; its only mismatch is the register order of the final halfword OR inputs. `sub_08004018.c` has an exact loop body but mismatches the loop entry and offset formation.

Inspected and skipped without a candidate check: `08011350` (many stack arguments) and `08018C48` (many stack arguments and register pressure). Ghidra pseudo-C was unavailable because `gen/ghidra/decomp.c` is absent. No symbols or names were added because the wrappers do not establish meaningful semantic names.
