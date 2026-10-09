# Push20 Astra motion escalation

Branch `codex/push20-astra-motion`, starting at `5ff44e3`.
Owned seed `sub_08160EA4` and siblings `081616D0`, `08161AF4`, `08161F18`.

No matches accepted; zero new emitted bytes. The retained ignored candidate
`wip/sub_08160EA4.c` emits 1,060 bytes, with four instruction differences:
`08160ECE/ED0` use r1 instead of r3 for the random-table address, and
`08160EE2/EE4` use r3 instead of r4 for offset 0x58c. The rest matches.
Siblings were not ported because the seed does not match.

Ten fresh manual checks tested a table-index inline helper, table struct,
object struct direction field, separate radius result local, explicit random
counter local, byte-pointer address arithmetic, an inline radius calculation,
an inline raw-offset table-load helper, unbound register local declarations,
and an inline direction helper. None improved the four-instruction baseline;
array indexing moved the table load before the shift, and a separate radius
result or inline radius calculation moved the post-call addition too early.
All sources and full check output remain ignored under `wip/`.

A bounded 90-second single-worker permuter search used the fresh inline
raw-offset table-load helper (`wip/table_cast_helper.c`). Base score 20;
run directory `build/permute/sub_08160EA4/run-b89ru1v4`. An internal assertion
reported duplicate AST nodes (seed 30850471491104726194). No unsafe candidate
was promoted. No inline asm, bound registers, volatile operations, artificial
branches, data changes, or semantic names were introduced.
The search ended normally at its time limit with `no candidate output saved`.

Validation: `build/venv/bin/python tools/build.py` printed
`build/boktai3.gba: OK`. Only this note is committed; no source was accepted.
