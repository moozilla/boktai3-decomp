# Sol family push 3

Base `a2841f3` on `codex/sol-family-push-3`. Ownership `080A0000 <= addr < 08170000`, excluding `08160EA4`, `081616D0`, `08161AF4`, and `08161F18`. Started 2026-10-09 03:28:22 UTC; bounded to 20 active minutes or 25 new matches. No delegates, push, PR, screenshots, make, shared generation, data definitions, semantic names, forced assembly, or register bindings.

Eight new verified matches, **1,688 emitted bytes**. Every source passed `check.py`; five complete builds covering the respective batches printed `build/boktai3.gba: OK` before the individual translation-unit commits. Already-C `080A4940` was accidentally checked from its retained WIP during triage but was explicitly excluded from the count.

| Commit | Function | Emitted bytes |
|---|---|---:|
| `591a83a` | `sub_080BB4DC` | 208 |
| `a244ed2` | `sub_080C61AC` | 208 |
| `1ec8314` | `sub_080E0390` | 208 |
| `92ef33c` | `sub_080A0054` | 260 |
| `fd85abc` | `sub_080A090C` | 208 |
| `faabf94` | `sub_080B18AC` | 140 |
| `509a225` | `sub_080CBE40` | 228 |
| `c5f5ba8` | `sub_080E3790` | 228 |

## Accepted source findings

`080BB4DC`: retained seed was a global r4/r5 swap. A 90-second permuter search improved 115 -> 0 by adding a do/while(0) scope around the final mask/store pair. Manual byte verification accepted it. Signed offset preserves ordinary pointer arithmetic in `s - (-off)`. `080C61AC` and `080E0390` ports both matched. Exact/loose sibling `08086A2C` lies outside worker ownership and was not ported.

`080A0054`: 116-instruction handler, 260 bytes. Struct fields preserve leaf IP allocation. A static inline u8 predicate groups null/flags/z checks and returns explicit 1/0. Separately assigned u8 zero and pointer-before-one setup preserve distinct constants. Initializing constant three before state byte writes lets the compiler derive the later mask as observed. Final other-object pointer must be calculated before mask -5, avoiding an early subtract. Approximate ten manual checks plus an obsolete-seed 90-second permuter search (60, no improvement); solved manually while that search ran.

`080A090C`: retained WIP had wrong flag mask (80 instead of 100), wrong queued field (2C4 instead of 2B1), and wrong callback-pointer type. Uses the verified struct initialization layout. Inline u8 predicate accepts the object and a pointer-out argument: compute `extra`, test its +15 byte, write the out-pointer, then return explicit 1/0. That naturally delays the extra-pointer copy until after the AND, recovering the original two-byte copy and full layout. Approximately ten checks. An accidentally launched search against the already-matching source was interrupted safely; not used for acceptance.

`080B18AC`: retained draft contained wrong queued offset and overly early mask/zero constants. Reused the verified struct initialization pattern with separate wide zero, u8 zero, u8 one/code, taken-pointer before zero initialization, and constant two before state writes. Exact in one corrected-draft check after initial triage. `0808A880` is an exact sibling outside scope; parent informed for possible port.

## Nonmatching retained findings

`080A3E28`: first 90-second search 50 -> 20. Reusing outer reset literal local eliminates redundant copy; scalar mask temp fixes flags-pointer allocation. Three instruction differences remain: initial mask/load r0/r1 swap. Four manual variations did not fix; another 90 seconds from score20 produced no candidate. WIP and ignored `build/3e28-score20.c` retained. No match.

`080EB920`: 90-second search 935 -> 365. Taking an address of the parameter and reading its struct pointer through it removes cached done-address allocation. Moving wide-zero initialization after taken pointer fixes store setup order. Remaining queued address is still cached in r6, giving extra push and four-byte size gap versus original recomputation. Pointer-out alias at final queued write did not fix. Approximately four checks. Safe corrected WIP retained.

`080B0728`: drafted 149-instruction seed, loose sibling `080C2E14`. 90-second search 1580 -> 830. Low-scored output changes the callee return type and uses an uninitialized offset; reject it. Original faithful WIP remains with cached done address, wrong callback-offset derivation, and second-state initialization allocation differences. No port or match.

`080CBE40`: new 112-instruction / 228-byte seed with loose sibling `080E3790`. Initial 90-second mixed-alias draft search 625 -> 300, then manual work removed extra preserved register, made code u8, assigned zero after taken pointer, and kept explicit unsigned field offset live across callback setup. This reproduces separate callback mov/lsl rather than deriving it from code offset. Search from score140 improved to70: signed `off` plus `obj - (-off)` for the final halfword destination recovers copy allocation. Next 90 seconds 70 ->10: do/while(0) scope around the first done-byte store solves eleven register differences. Only timer destination r1 versus original r0 remains (two instructions). Scope around timer store worsened allocation; separate timer pointer local unchanged. More than ten checks were used because each successive seed materially improved a two-member larger family. Final 60-second drain search pending at note creation; record result below. All retained in ignored WIP/build only.

`08131B40`: three-member family; typed object/callback/index access did not stop callback offset deriving from index offset. 90-second search 470 ->315, worse than prior round's retained best205. Two checks, no match. Kept WIP for later type/helper work.

`080C0750`: changed zero16 to u8 zero, code to u8, compound-mask destinations, and made halfword source load occur before destination calculation. Still coalesces the two zeros into one, omitting original second zero and changing preserved registers. Three checks including a C89 declaration correction. No match.

`080B41A0`: original retained draft had inverted z predicate, wrong other-pointer offset F4 instead of3D0, and wrong u32 return (original void). Reused exact 080A2048 helper to correct semantics, then tried saved second-argument local; register allocation still leaves argument in r1 instead of copying it. 60-second search 280 ->0 and check reported MATCH, but output tests uninitialized `saved` in `if (s || saved)`; **rejected despite byte equality**. Safe variants using initialized `arg` or a do/while(0) copy do not match. Six checks, safe faithful WIP retained only.

Read-only triage of `080A2A38` and `080A4260` shows the same offset-derivation-heavy setup family; neither drafted. No exact SDK/source evidence found or claimed; this round focused game handler families. Approximate 60 manual byte checks across seeds/ports/triage, plus bounded searches above; all unsuccessful drafts remain ignored. No full clone sweep.

Final drain correction: `080CBE40` **matched** (228 bytes), superseding its nonmatching result above. Last 60-second search 10 ->0. Assigning the signed `off` local inside the code-byte index (`off = field`) before its later independent overwrite changes the dying timer-pointer allocation to original r0. Both locals are initialized on every read; signed offset keeps negative-expression pointer arithmetic bounded. Byte verification accepted the safe source and loose owned sibling `080E3790` (228 bytes). No out-of-range sibling for this pair. Fifth complete build printed `build/boktai3.gba: OK` before individual commits `509a225` and `c5f5ba8`. No new search after this drain.

Final result: **eight new matches, 1,688 emitted bytes**, final source tip `c5f5ba8`. End 2026-10-09 03:47:42 UTC, **19 minutes 20 seconds elapsed**. Working tree clean after this notes commit. No active permuters or further new work. Parent notified of exact out-of-range siblings `08086A2C` and `0808A880` for its independent port/integration decision. Prior rounds' 13 matches are not included in this round's eight.
