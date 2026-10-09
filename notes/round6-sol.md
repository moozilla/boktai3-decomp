# Production round 6 — Sol

Baseline: `1c294ea94a95dc358714a17fe8ced4d2610df5fe`. Isolated branch: `codex/endurance-r6-sol`.

Nine net new functions, 932 emitted bytes: four manually solved seeds and five automatic clone ports. No baseline rewrites. Matching and required full builds occupied 14.10 minutes through the result manifest. All ownership exclusions, including the additional ten MGS starts, were respected.

| Family seed | Members | Bytes |
|---|---:|---:|
| sub_08170EE8 | 2 | 216 |
| sub_082232A0 | 2 | 352 |
| sub_0819D718 | 2 | 160 |
| sub_0818E1BC | 3 | 204 |

Three complete `tools/build.py` runs printed `build/boktai3.gba: OK`, before each group of individual TU commits. No make or generated-data regeneration. Parent owns combined emulator validation.

Logged checks: {'mismatch': 24, 'match': 9, 'error': 2}. The readelf failure was a diagnostic overlap with full linking; the identical candidate was checked again after linking completed. One invalid C89 declaration order also failed compilation. Compiler/check elapsed time: 18.62 seconds. No target exceeded ten distinct manual checks. All sources, timestamps, transcripts, source hashes and ordered commit SHAs remain in ignored `build/round6/`.

Useful patterns: branch-specific inline coordinate setters preserve repeated constant loads that shared assignments merged; nonvolatile narrow arrays retain the original read/OR/write in agbcc whereas volatile introduced a second read. For state latches, constructing the flag pointer before zero and loading the kind before its field pointer fixed allocation. For the three packed-flag latch variants, a separate `value=mask; value&=old; zero=0;` preserved mask register allocation; early return from the u8 inline helper preserved the original common comparison branch.

Failures retained: fresh renderer `wip/round6/sub_08218508.c` (784 versus 788 bytes), bounded 90-second permutation score 2235 to 2020; `wip/round6/sub_08194858-best244.c` (244-byte target, remaining r4/r5 and temporary allocation differences), permuter aborted a duplicate-AST assertion; `wip/round6/sub_0819E1EC.c` (112-byte target) and best 60-second permutation score 465; timer and small-grid initializers preserved as well.

Scorer caveat: the earlier packed-flag source scored zero in `build/permute/sub_0818E1BC/run-9btmgbde/base.c` although check.py found an unconditional branch skipping the common compare. Only direct MATCH sources were accepted; ordinary C early-return shaping repaired this branch. Parent was notified.
