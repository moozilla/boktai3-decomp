# Luna B production round 2

Baseline `e2546f6`, branch `codex/endurance-r2-luna-b`, tip `c3231fb`.
Assigned `080A0000–08170000`. Matching interval: 2026-10-09 02:07:48–02:27:46
UTC (19m58s, includes build waiting).

Integration accepts **21 new functions, 944 emitted bytes and 980 progress-span
bytes**. The worker produced 22 matching commits but `0816B3C0` was an existing
function rewrite, excluded by the orchestrator. All accepted matches were manual
C or manual family adaptation; the worker corrected its earlier claim of two
automatic clone ports to zero. Full ROM builds passed before the matching commits;
the orchestrator validates the combined batch and shift tests.

Accepted addresses: `0816C1D4`, `0816D394`, `0816D308`, `08164738`, `0816475C`,
`08164780`, `0816D47C`, `0816D4A4`, `0816CDA4`, `0816CEA0`, `0816D618`,
`0816D7F8`, `0816E9F8`, `0816E540`, `0816BB10`, `0816BC2C`, `0816B354`,
`0816E408`, `0816D91C`, `08166A94`, `0816CEB4`.

Retained failures: `0811E8B0` family (four checks); `0816D40C` (two);
`08165870`/`0816589C` (three each); `080C47B8`, `08115A6C`, `08163E74`,
`08166CD0`, `08167508`, `0816B390`, `081218A0`. Candidates and private logs
remain in the worker's ignored `wip/` and `build/round2/` directories.
