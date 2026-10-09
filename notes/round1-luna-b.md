# Production matching round 1 — Luna B

Baseline `540daab`; range `080A0000 <= addr < 08170000`.

Eleven new matches, 640 emitted/progress-span bytes: `080A4940`, `080AB010`,
`08140328`, `08140364`, `0814039C`, `081403D8`, `0816C9AC`, `0816CD6C`,
`0816C1A4`, `0816CECC`, `0816D4BC`. The first six were followed by a bounded
five-function continuation. One exact automatic clone port produced `081403D8`;
the others were authored or adapted manually. Full ROM builds passed before
each per-function commit group.

The proposed rewrite of `080A06C0` was excluded: baseline C already matched.
Always consult the current worklist/source inventory before drafting a target.
The first pass's reported elapsed estimate is unreliable; the continuation
has actual UTC timestamps 01:09:18–01:12:27 on 2026-10-09 (3m09s, October 8
Pacific). Private checks and logs remain in `build/round1/` in the historical
`match-astra-pilot` checkout, now on `codex/production-luna-b`.

Useful patterns: nested table/bitfield indexing and a static inline helper
that materializes a bit test as a boolean before a second condition.
Retained failures: `080A0680` (address formation/store order), `080AB4E0`
(loop register allocation), `08140834` (division/result order), `080A3C78`
(stack vector), and `080A0894`/`080A3C00` (boolean/field addressing).
