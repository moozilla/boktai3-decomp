# Sol family push 5

Base `a2e18e2` on `codex/sol-family-push-5`. Parent expanded scope to `08000000 <= addr < 08170000`, retaining exclusions `08160EA4`, `081616D0`, `08161AF4`, and `08161F18`, and excluding parent RFU/SDK work. Started 2026-10-09 03:54:06 UTC; bounded to 15 active minutes or 20 matches. User weekly floor was extended to 35% remaining. No delegates, push, PR, screenshots, make, shared generation/tool mutation, data/name changes, register bindings, or forced assembly.

Nine new matches, **1,608 emitted bytes**. All passed manual byte verification. Three complete builds on the respective batches printed `build/boktai3.gba: OK` before separate one-TU commits.

| Commit | Function | Emitted bytes |
|---|---|---:|
| `38f7a85` | `sub_08061F18` | 148 |
| `dc1bb27` | `sub_08061FAC` | 148 |
| `3041b77` | `sub_08062040` | 148 |
| `77ec8e0` | `sub_08084100` | 128 |
| `59809b7` | `sub_08093718` | 128 |
| `de0f8f1` | `sub_0809D3A4` | 128 |
| `6f03e01` | `sub_0808CE50` | 260 |
| `1ba1f9e` | `sub_0808CF54` | 260 |
| `65035f3` | `sub_0808D058` | 260 |

Fresh-family triage ranked only wholly unseeded clone families by aggregate instruction count within the expanded scope; this was not a matched-source port sweep. Chosen seeds span the new earlier range. `080A3E28` was not retried.

`08061F18` / `08061FAC` / `08062040`: natural nested switch with cases 4/5, 1, 2 and inner result switch 0, 1, -1. Shared error label preserves cross-case branch and sound-call order. Representative matched first check; two loose ports both matched. No permuter solution needed. An inadvertently launched already-matching-source search was safely interrupted through wrapper SIGINT and its process group cleanup before the full build; no output used.

`08084100` / `08093718` / `0809D3A4`: inline u8 predicate preserves materialized bit-2 boolean. Cast pointer fields and compound constant-register mask operations preserve flags operand order. Failure label before success gives original unsigned comparison branch; signed absolute distance and threshold 200 follow observed assembly. Representative matched first check and both loose ports accepted. No semantic naming inferred from this behavior.

`0808CE50` / `0808CF54` / `0808D058`: 120-instruction, 260-byte family. Reused verified handler structure and CBE40 initialization layout. First natural draft was four bytes short because compiler cached the done-address across the call. An address-of-parameter read through an outer reference introduced stack accesses and was worse. First 90-second search improved3995 ->545, eliminating stack reference and using `(*(&s))->b` in later state setup, which prevents the cached done pointer. Semantic audit: candidate reordered callback/timer stores in first setup and sank the zero; it was not accepted as-is. Restoring observed callback-before-timer order and assigning wide zero after callback load recovers all but two register differences. Second90 seconds from score10 had no candidate. Promoting signed offset into outer function scope and assigning it from the first code-field offset, then overwriting it before the later callback-field lookup22C, fixes timer destination allocation. Every read is initialized and pointer arithmetic is bounded; observed call/store order preserved. Representative and both sibling ports byte MATCH. About six representative checks plus two ports. Actual emitted260B, larger family selected for native byte yield.

`08063A68` / `080640F4`: new sign-preserving multiplication/shift coordinate draft. Struct fields retain IP; inline multiply/shift handles nonnegative arm first and truncated negative magnitude. Changing scale to u16 and explicitly loading it before the second table lookup restores memory read ordering. Base pointer124 with `[1]` recovers original flags128 base+4 access. Still omits a two-byte initial scale copy and uses different result/pointer registers.90-second search355 ->315. Semantic audit: best adds an `if(s)` whose both branches write the same field; no improvement sufficient for match, so original safe source retained. Two checks; no port or accepted source.

`0808A90C` / `080C49BC`: new100-instruction seed drafted from bool/init patterns. Four-byte size difference: halfword destination418 derives from416 while original materializes independently. Extra preserved register remains from three live explicit offsets and state one/zero allocation. One check; no search or acceptance.

`0802B070` / `0802B180` / `0802B240`: new54-instruction family. Bool helper consumes queued and done flags. Pointer-out first variant adds a two-byte pointer copy; simpler typed helper matches original control flow without it. Explicit pointer increment into count recovers base-relative store. Local code constant before assignment fixes code/pointer order. State constants and pointer still allocated in different registers.60-second search515 ->305. **Semantic audit rejected its best output: pointer `p` loses its initializer before `p++` and writes, an uninitialized read.** Safe latest helper variant retained; four manual checks, no ports or match.

`080829C0` / `08091E58`: new78-instruction seed using verified blocked2048 predicate. Initial outer mask local kept first argument in preserved register; scoping it only before first call recovers exact entry allocation. Explicit byte offset before table pointer reproduces table load ordering; random u16 fixes low-register allocation. Natural clamp `if(value<20)` emits cmp19/bgt rather than target cmp20/bge. Inline void helper taking address of the initialized local value and minimum20 produces exact signed clamp with no stack traffic. Eight register differences remain in table-base literal and final other-flags pointer setup. Five checks. Bounded90-second search on improved source remained score50 and saved no candidate output. A later outer-offset manual variant worsened table registers and was reverted. No wider seeds after this drain.

Approximately26 manual checks including accepted ports and failed drafts, three improving bounded searches, two non-improving searches, and one safely interrupted already-matching-source search described above. Failed candidates stay ignored under WIP/build only. Final drain result follows below.

Final result: **nine new matches, 1,608 emitted bytes**, final source tip `65035f3`. End 2026-10-09 04:07:08 UTC, **13 minutes 2 seconds elapsed**, within15-minute cap. All accepted semantic audits passed: initialized local reads, signed bounded pointer arithmetic where subtraction expressions appear, no changed call order, and exact emitted bytes. No new seed after final search drain; no active permuters. Safe corrected failed drafts and prior candidate snapshots remain ignored. Clean working tree after this notes commit.
