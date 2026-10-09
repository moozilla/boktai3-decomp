# Push20 high worker

Worktree: `/Users/benjaminharris/git/wt/push20-high`, branch `codex/push20-high`, base `5ff44e3`.

35 byte-identical functions, 5824 emitted bytes. Every source passed `tools/check.py`; accepted batches passed full `tools/build.py` before separate one-TU commits. No naming or data changes. Parent owns integration and shift regression. Final build log: `build/high-wave-final-build.txt`.

| Function | Emitted bytes |
| --- | ---: |
| 0820B174 | 228 |
| 0820D934 | 92 |
| 0820DE60 | 128 |
| 08210D60 | 292 |
| 08211288 | 108 |
| 0821159C | 268 |
| 08211C78 | 196 |
| 08212170 | 436 |
| 08212424 | 176 |
| 08212870 | 156 |
| 082131E4 | 176 |
| 082133BC | 332 |
| 08213810 | 120 |
| 08213DCC | 436 |
| 082141C0 | 156 |
| 0821425C | 128 |
| 082259A0 | 140 |
| 08226670 | 100 |
| 082266D4 | 116 |
| 0822780C | 56 |
| 08227ECC | 112 |
| 08228508 | 64 |
| 0822867C | 100 |
| 0822B1D8 | 88 |
| 0822B298 | 96 |
| 082286E0 | 108 |
| 0822874C | 204 |
| 08228848 | 224 |
| 08228EA0 | 128 |
| 08228F40 | 92 |
| 082290F4 | 140 |
| 08229258 | 296 |
| 08229380 | 164 |
| 082329D4 | 96 |
| 08232ED4 | 72 |

Useful source forms:

- RTC29258: direct global struct fields removed the validation pointer/value register swap; the retry body was already exact. RTC290F4: assign the default flag in the final argument expression. RTC28848: a distinct base pointer for the final comparison/counter preserved the original base literal and offsets.
- 12170: keep the base pointer and each branch's final flag pointer as distinct lifetimes. This removed the entire r5/r6 swap without permuter output.
- Cleanup loops12870/13810/11288: calculate the final two cleanup pointers before the loop. Setup12424 reused the matched133BC prefix and matched first draft.
- B174: calculate the signed byte offset before the table base, and initialize the shared zero after evaluating call arguments. 28508: accumulate each decoded decimal digit before loading the next one.
- Script26670/266D4: ordinary unsigned16-bit fields in32-bit containers reproduce the packed-coordinate assignments. Callees read only the three16-bit fields; the fourth field is padding.
- Direct compound16-bit MMIO writes and narrow temporaries mattered in141C0. Explicit DMA structures and readbacks matched the other initialization routines.

Bounded retained WIP:

- `082285C4`:184B source, only argument setup order differs at285F0/28632: the400 constant is materialized after copying year to r0. Safe inline `yearDays` fixed all register allocation. About10manual checks;30s permuter score120, no output.
- `0820267C`:144B source, only CpuSet source-address/control-constant setup order differs.30s permuter score65, no output.
- `082158B0`:180B best source, first halfword load/255 constant order reversed. Flat structure removed the wrong array-base optimization; retained snapshots.
- `08201160`:272B safe source; redundant pool-to-r0 copy and stack argument layout remain. Most setup/copy instructions align; retained snapshots.
- `08213FD8`:360B versus364B, shadow/affine pointer lifetimes and mode constant branch differ. Four manual drafts; no permuter.
- `08215400` (`ObjGfx_LoadTiles`):140B versus144B; outer-loop counter/next-entry spill choice differs. Two drafts.
- `082157EC`: graphics reset loop allocation and loop reversal; three drafts.
- `08210CB0`: correct176B shape, two addition operand orders;90s permuter score20, no output. `08229180`: last stack parameter hoisted into a register and retry index spilled. `08201784`: packed vectors/halfword updates remained mismatched.
- `08231EF8` family:228B structure/late loop matched, initial scaling/register choices differ.90s permuter encountered duplicate-AST assertion, no accepted output. Tail inspection supported a four-slot coordinate/effect family, not a codec hypothesis.
- `0823652C` family:112B palette interpolation structure, register/order differences.90s permuter575→350, no match. No repeat of previously exhausted038A8/18508 families.

Fresh seeded loose-clone ports:0. No permuter candidate was accepted. Candidates remain under ignored `wip/high/`; no nonmatching C was committed.

Boundary coordination:

- Parent owns the hidden tails08229424 and08232F1C. Our29380 is164B and32ED4 is72B, ending immediately before those tails.
- Sent parent hidden candidates08212F34 (flag setter after12F24) and08227F3C (16B),08227F4C (12B),08227F58 (4B), the flag setter/getter and zero-return after27ECC; parent corrected the original literal-word misidentification. No boundary files were edited here.
- Parent owns29440–2B13C,4930C,00A10, and>=49558. No overlap.

Tooling: a standalone check can race the final ELF rewrite of a full build and report a readelf magic-number failure. Such checks were rerun after build completion; they were never treated as match evidence. All build outputs were private; shared generation/environment/tools were read-only.


## Authorized eight-minute follow-up wave

Two additional exact audio wrappers:0822B1D8 (88B),0822B298 (96B),184 emitted bytes total. Worker total is35 functions/5824 emitted bytes. Both use the existing MP2K song/music-player structures and labels. Explicitly loading the active song into a local before the conditional fixed the otherwise exact register assignments. B180 was already tracked and matched; the redundant draft was restored and contributes no new bytes.

Wave started05:46:22 UTC. Meter at05:50:18 UTC remained23%; no resets were used. Final wave build log is `build/high-wave-final-build.txt`. No new searches or permuter runs.

Fresh retained WIP: B6C8 slot initializer192B versus152B, incoming-register/stack-lifetime misses after three drafts; B7E4 finder176B versus168B with incoming-register/stack-lifetime misses after two drafts; B5FC64B versus68B branch/literal-island layout after one draft. No repeat of prior near-matches. Hidden B760 appears to be a40-byte wrapper before B788; sent parent for optional later boundary review, no metadata edited here.
