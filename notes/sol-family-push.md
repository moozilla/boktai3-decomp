# Sol family push

Base: `7385347e42bbc64908df29ca375b044e862927aa`; branch `codex/sol-family-push`. Own `080A0000 <= addr < 08170000` excluding `08160EA4`, `081616D0`, `08161AF4`, `08161F18`. Started approximately 2026-10-09 03:00 UTC; bounded to 20 active minutes or 40 matches.

Verified seeds and ports so far: `080A2048`, `080A5B8C`, `080ACCF0`, `080E7D08` (108 emitted bytes each), `080A6030`, `080AD1C0` (176 each). Six matches, 784 emitted bytes. Each passed check.py, then the complete build printed `build/boktai3.gba: OK`; committed individually.

`080A2048`: the retained permuter source had the right allocation and mask load order but a false zero score because it branches directly past the materialized predicate comparison. Returning 1 and 0 directly from the static inline u8 predicate (instead of common local result with goto done) preserves the intermediate branch and matches exactly. Four owned exact siblings accepted; `08085D58` is outside scope.

`080A6030`: retained WIP used incorrect fields `2AC` and `2B1`; assembly uses `2B0` and `2B5`. Set function-pointer and code locals after the call instead of declaration initializers to avoid hoisting into preserved registers. Make consumed flag local u32 to avoid an extra duplicate narrow-value register. Separate flags/mask locals reproduces mask load before read. `080AD1C0` exact port accepted; `08086B6C` outside scope.

`080BB4DC`: three-member loose family with `080C61AC`/`080E0390`, 208-byte representative. Approximately ten manual checks and 60-second permuter (550 -> 175) leave s/arg r4-r5 swap plus short-lived flag register differences. Shape otherwise correct; retained WIP wraps Body helper as final test, best raw source is ignored build/bb4dc-base.c and permuter output-175-1. Not matched.

`08131B40`: three-member family `08131BC4`/`08131C48`. Mask-before-global-load solved with no-arg inline helper. Most early control flow exact. Remaining constant-address register allocation in writes; callback-table field offset derives from prior byte-index offset (`subs #26`) when original uses separate mov/lsl. 60-second permuter improved 470 -> 205, but only register/derived-constant differences remain. Retained WIP, not matched.

`0811E8B0`: corrected retained draft's duplicate precondition to do-while and exposed live second argument. As earlier notes predict, compiler caches count address in r7 while original recomputes s+54. Integer address conversion did not stop CSE. Retained WIP, not matched.

`0816B390` matched (48 bytes): `FDFF` in retained WIP produced wrong high mask halfword. Pointer-first local plus u32 `~0x200` preserves the original full-width `FFFFFDFF` literal and mask load before halfword read. Independent compiler correctness, no naming inference.

`08115A6C` matched (44 bytes): initial mismatch was pointer register r1 instead of r2. Assembly leaves r1 live across setup and passes it to `08115A20`; adding the hidden second arg fixes allocation. Matched callee `src/fn/sub_08115A20.c` confirms it dereferences its second argument's +4 field. Local pointer then constant mask 1 reproduces mask-before-read.

`080A0894` matched (120 bytes): pointer computed before predicate constant; failure label before success block keeps original forward `blo`; update masks written as `mask |= *flags` rather than compound memory OR preserve original constant-register destination.

`080A3C00` matched (120 bytes): reused 080A2048's now-exact bool helper. Prior draft inverted positive-z predicate and cleared `~3`, whereas assembly has return zero for z > 0 and mask `-3` (clears bit 1). Struct/cast flag pointer with full-width -3 reproduces original.

`080AABEC` matched (160 bytes): struct S/T field topology produces original `mov ip,r0` leaf. Predicate then flag pointer local prevents address CSE at wrong point. Struct flags pointer must be assigned before mask local to keep target mask/read registers. Final flag mask is -3, as in assembly, not prior WIP's ~3.

`081218A0` skipped after switch draft: switch now reproduces cmp2/bgt then cmp1/blt body exactly; compiler removes original redundant entry copy r1=r0, yielding 28 instead of 32 bytes. Retained WIP. `08163E74` skipped: retaining first parameter in local and repurposing a as loop index fixes allocation, but adds redundant save/copy pair, so still nonmatching.

Final search correction: `08163E74` **matched** (44 bytes), superseding its skip above. 60-second permuter reached score zero and byte verification accepted. A separate uninitialized local assigned from first argument before the loop preserves desired allocation; reusing the argument as the loop index did not. Source cleaned only by renaming generated local to `saved` and verified again.

New larger drafts retained: `080C0750` (two-member family): two zero values coalesced and original separate r5 zero is absent; field setup and halfword-copy ordering still differ. `080EB920` (two-member loose family): correct struct/IP access, but compiler retains done and queued field addresses in r4/r6 where original recomputes; remaining fields approximately correct. `080EBC38` (two-member loose family): flag test and initial sound call shape correct, but branches lay out clear-mask path first and constant offset is not kept in r6. No partial source copied into src/fn.

`08167508`: removed erroneous volatile counter from retained draft (it reloaded instead of truncating postincrement) and split coordinate adds into explicit loads. 60-second permuter 60 -> 55, still 11 register differences; counter pointer occupies r3 rather than target r1, displacing y and coordinate temporaries. Final struct variant checked separately below.

Final result: **12 new matches, 1,320 emitted bytes**, all separately committed. The final complete build on the combined twelve source files printed `build/boktai3.gba: OK`. No shared generation, push, PR, names, or data-layout edits. Parent owns combined shift-test review. End approximately 2026-10-09 03:19 UTC, about 19 active minutes.

| Commit | Function | Emitted bytes |
|---|---|---:|
| `b57cb5e` | `sub_080A2048` | 108 |
| `76f3be6` | `sub_080A5B8C` | 108 |
| `d3eb4ab` | `sub_080ACCF0` | 108 |
| `7acd606` | `sub_080E7D08` | 108 |
| `81022f2` | `sub_080A6030` | 176 |
| `17c5edc` | `sub_080AD1C0` | 176 |
| `6b291a7` | `sub_0816B390` | 48 |
| `248f143` | `sub_08115A6C` | 44 |
| `55ed06d` | `sub_080A0894` | 120 |
| `4ab4293` | `sub_080A3C00` | 120 |
| `2f7a704` | `sub_080AABEC` | 160 |
| `b5ecf64` | `sub_08163E74` | 44 |

Final struct variant of `08167508` has the same eleven register differences. Retained corrected struct source in ignored wip; best cast source also retained in ignored build/67508-best.c. Neither matched. Final separate uninitialized s alias on `080BB4DC` only changed initial copy ordering; range-wide register swap remains, and no source was accepted.
