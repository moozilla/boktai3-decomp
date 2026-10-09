# Sol family push 6

Base `71b20a1` on `codex/sol-family-push-6`. Started 2026-10-09 04:08:58 UTC; parent authorized at most four active minutes or five new matches, weekly remaining36% with floor35%. Same expanded game-code scope and reserved motion/RFU/SDK exclusions. No extra agents, push, PR, screenshots, make, shared generation/tool writes, or repeated080829C0 search.

Fresh-family triage read only small wholly unseeded families suitable for verified flag/callback templates. `08083A8C` and `08036EC8` were read but not drafted. Chosen representatives `0802A758` and `080F6118` are independent handler candidates, not SDK-source claims.

`0802A758`: natural C clears pending byteB1 then invokes297FC(s,2,1,0), zeros word178 and byte17C, ORs flags9C with12, and dispatches callback108. Local pointer-before-zero and scoped mask match observed setup allocation. Representative MATCH80B on first check. `0802A87C` loose port also MATCH80B. `0802A82C` loose port initially mismatched only final297FC call argument: source's0 conflated with other zero immediates and port kept0 while assembly has8. Direct evidence corrected call to297FC(s,11,1,8), then MATCH80B. No other semantics altered. Complete build printed `build/boktai3.gba: OK` before individual commits `f99e733`, `abae6be`, and `509d8d1`.

`080F6118`: inline u8 predicate tests flags119 bit1, clears it and done12E, then conditionally installs callback174 and invokes it. First draft88B correct size/control flow but thirteen register differences, mask/value r0/r1 swap. Explicit mask &=value before test unchanged. Byte-read u8 field type alone was unchanged. Final manual variant separates u32 test mask1 from inner s32 clear mask-2 rather than reusing one variable across the condition; this restores exact r0/r1 allocation and MATCH88B. Both masks and value are initialized, bit clearing and callback behavior remain unchanged, and no call/store ordering changed. Four seed checks, no permuter search or false-match acceptance.

Parent notified at floor35% to drain only existing work. The already-underway `080F6118` seed completed its final manual variant at04:11:33 UTC; sibling ports6170/61C8 were **not started after the drain instruction**. Second complete build printed `build/boktai3.gba: OK` before fourth source commit `c2b1d09`. No new targets or rounds.

| Commit | Function | Emitted bytes |
|---|---|---:|
| `f99e733` | `sub_0802A758` | 80 |
| `abae6be` | `sub_0802A82C` | 80 |
| `509d8d1` | `sub_0802A87C` | 80 |
| `c2b1d09` | `sub_080F6118` | 88 |

Final result: **four matches, 328 emitted bytes**, source tip `c2b1d09`. End 2026-10-09 04:12:39 UTC, **3 minutes 41 seconds elapsed**, under four-minute cap. Eight manual byte checks (four on6118 and four on2A family including corrected port), two successful complete builds, no permuters. Working tree clean after notes commit. No new work after drain; no active tools.
