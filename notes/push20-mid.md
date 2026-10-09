# Push20 mid-range worker

Branch `codex/push20-mid`, base `5ff44e3`. Owned `[08100000, 08200000)`;
08160EA4 / 081616D0 / 08161AF4 / 08161F18 were reserved to the parent.
Read README, CLAUDE, HANDOFF, WORKER and the latest Sol round/family notes.
No benchmark, pushes, PRs, shared header edits, semantic symbol renames or data
units. Only exact C translation units were added. Counts below include emitted
literal pools, as reported by check.py.

## Accepted

23 functions, **4,824 emitted bytes**. Each passed standalone check.py MATCH;
complete build OK preceded its individual one-TU commit.

| Function | Bytes | Commit |
| --- | ---: | --- |
| 081CC690 | 84 | d2dd559 |
| 081094EC | 96 | 37d7dba |
| 0810BE98 | 96 | 479854f |
| 0810CBC8 | 96 | f063e5c |
| 081155FC | 120 | 725adce |
| 08115D58 | 120 | 25d5d72 |
| 0812CD10 | 96 | beef77e |
| 0812D46C | 96 | 1680724 |
| 08175EEC | 308 | 4440415 |
| 081742F0 | 260 | e311cf8 |
| 0817565C | 504 | 91ffcc7 |
| 081E0B4C | 360 | 5012896 |
| 081EC204 | 268 | 476743b |
| 081DBE04 | 404 | d2404fb |
| 081EA8A8 | 268 | 1f4abcd |
| 0816CC88 | 228 | 48b322d |
| 081AE008 | 184 | 1720d4e |
| 081A7040 | 184 | c9cd320 |
| 0810F8D8 | 184 | fe2a851 |
| 081D5C08 | 216 | 49211e0 |
| 0819A5AC | 176 | 01bf890 |
| 081E2E7C | 200 | c0fdf2d |
| 081C5C2C | 276 | 6d53060 |

## Reusable source observations

- Loose clone ports yielded the 081094EC/BE98/CBC8 and 081155FC/15D58 and
  0812CD10/D46C groups. Ported literal-pool constants still needed manual review.
  Latest full owned-range loose clone port scan found no further accepted ports.
- Call-heavy constructors yielded most native bytes. Separate locals for
  distinct resource/object lifetimes often repaired swaps of preserved registers.
  Conversely, reusing the resource variable across resource loads repaired
  081E0B4C/0819A5AC copy setup; initial background resource and later object
  resource should use distinct locals in 081E013C.
- Assigning zero or a persistent size in the relevant call/store expression
  preserves the observed load order (6CC88 index pointer, EC204 palette size,
  A7040 final pointer, EA8A8 two/one argument lifetimes).
- A named destination pointer followed by a named u16 value avoids the extra
  halfword constant copy in C5C2C. Declaring only the narrow value first reverses
  literal-pool and store ordering.
- Small typed structs model observed word copies and fields. E2E7C uses a
  copied 32-byte object and a partially initialized coordinate object; no ROM
  bytes or data definitions were introduced. D5C08 uses a struct for offsets
  18/20/22 to retain the observed global base and field addressing.

## Retained bounded drafts

All nonmatches are ignored under `wip/push20/`. Do not promote without a fresh
check and full build. No register bindings or assembly were used.

| Candidate | Best state / next hypothesis |
| --- | --- |
| 081F0E24 | 324-byte body, only two CpuSet setup instructions exchanged; six manual checks, 90 s single-job permuter with no saved output. Scratch struct fixes the two stack offsets; distinct palette resource locals fix argument copies. |
| 08178FC8 | 460 bytes, seven final r0/r1 differences. Best retained `sub_08178FC8-best460.c` (and current main draft). Nine bounded checks; 90 s permuter reached score 40 but requires extra locals and did not match. |
| 0816F6B4 | `sub_0816F6B4-best40.c`, 180 bytes, only six initial flag-update instructions differ. Ordinary do-block grouping from the 90 s permuter was retained. Enlarged struct array to 266 elements (u8 index + 10), rechecked: same six differences. |
| 081DC470 | 424-byte manual body, nine initial x/y/half-pointer register differences; 90 s permuter no output. Current draft retains the u8 half-pointer version. |
| 081E013C | Revisited using learned resource lifetimes: 736 to 700 bytes. Current retained 700-byte draft has the correct long-lived zero/resource registers; initial offset allocation, first saved pointer and two halfword constants remain. About six total manual checks; no permuter. |
| 081BC820 | 352-byte body after splitting early/late zero. Remaining preserved-register rotation and some address choices; five checks, no permuter. |
| 0812642C | 324 bytes; u32 parameters plus use-site narrowing fix premature stack argument loads. Remaining high-register rotation and first returned-pointer use. Four checks, no permuter. |
| 081975C8 | Two checks; compiler merges the two intended long-lived zero values. Typed narrow locals did not separate them. |
| 081625C0 | Five checks; compiler replaces indexed flag writes with pointer induction and keeps an extra preserved zero. Short index introduced truncation. |
| 08125B68 | 84-byte best, first loop movs vs adds only; ~six checks and 60 s permuter. Reusing the mask eliminates a required second shift. |

Earlier short bounded failures: 08131B40 (3 x 132 B family), 081134B0 (2 x
132 B forwarding), 08105194 (bitfield stack merge), 0815C230 (2 x 140 B),
08171EFC (2 x 128 B), 081A543C (2 x 180 B), 08126000 (2 x 116 B).
081D5908 draft merged halfword initialization into a word store; no permuter.
081BF67C/08169504/081DEE38 inspected only: repeated packed stack initialization.
081BD38C and 081E23A0 inspected later: substantial stack-coordinate lifetimes
and repeated loops; no draft. 081E8374 is a seven-member 580-byte family worth
an expert pass; prior notes record 552-byte drafts but that old checkout was
unavailable. Do not repeat these without a new hypothesis.

## Boundary and tooling evidence

Skipped 08170A0E: non-word entry pushes r5/r6 without LR and appears to be a
split prologue. Skipped 08171F7C: body has trailing independently prologued code
inside the discovered span. Also observed trailing independently prologued raw
bytes at 081F2B70 after 081F2A08; left untouched. Parent owns boundary audits.

Complete build logs `build/full-clone.txt`, `full-batch1.txt` through
`full-batch10.txt` report `build/boktai3.gba: OK` for their accepted batches.
A deleted definition signature temporarily broke batch4 splitting; repaired
and standalone checked before restarting its successful full build. Occasional
standalone checks during full-link completion hit a transient readelf magic
error; repeated after completion, with no candidate accepted from such output.

An accidental `tools/disasm.py 08178FC8 > build/dis-78f.txt` invocation was
interrupted inside its first gbadisasm subprocess before assembly output was
returned. It rewrote only generated gen/disasm.cfg, which was reported to the
parent immediately for audit; worker made no restoration. No generator process
remained. Assembly stats were unchanged: code.s 20,234,095 bytes, timestamp
2026-10-08 19:30:49 PDT; code_sym.s 19,752,724 and data.s 2,963,498 bytes,
both 19:32:32 PDT. Subsequent inspection used read-only asmat.py.

Final complete build `build/full-final.txt` printed `build/boktai3.gba: OK`.
Snapshot is 23 functions / 4,824 emitted bytes through 6d53060.
