# Sol family push 4

Base `b216669` on `codex/sol-family-push-4`. Parent explicitly expanded ownership for exact siblings `08086A2C` and `0808A880`, then authorized at most five active minutes on one retained near-match. Started 2026-10-09 03:48:28 UTC. No wider seed hunt, delegates, screenshots, push, PR, make, or shared-generation writes.

`08086A2C`: exact port from verified `080BB4DC`, 208 emitted bytes; `check.py` MATCH.

`0808A880`: exact port from verified `080B18AC`, 140 emitted bytes; `check.py` MATCH.

Combined full build printed `build/boktai3.gba: OK` before individual commits `c987d00` (08086A2C) and `fb4161f` (0808A880). Two new matches, **348 emitted bytes**. Follow-up seed is retained `080A3E28`, score20 / three register differences. Plan is to reuse the source-scope/lifetime technique that solved round3's `080CBE40`, with any generated candidate checked for initialized locals and faithful call order.

Existing near-match work started after the full build at 03:49:21 UTC; five-minute deadline 03:54:21. Seven manual checks of `080A3E28`: assigning the outer timer-literal local during mask setup caused wider allocation changes; do/while(0) scope around first timer store left the original three differences; narrowing mask temporary to s8/s16 caused six differences; assigning loaded flags into the outer mask temporary also caused six; assigning mask result at final store left three. Retained safe score20 variant with initialized outer mask local and faithful call ordering. Bounded 90-second search on that final-store variant stayed score20 and saved no candidate output. No wider work begun.

Final result: **two new matches, 348 emitted bytes**. `080A3E28` remains nonmatching and is retained only in ignored WIP (`build/3e28-round4-base.c` also preserves the previous faithful seed). End 2026-10-09 03:52:15 UTC, **3 minutes 47 seconds total**, **2 minutes 54 seconds on the retained seed**, under the assigned five-minute cap. Nine manual byte checks total (two ports, seven seed variants), one 90-second search, one complete successful build. No active permuters or additional work.

| Commit | Function | Emitted bytes |
|---|---|---:|
| `c987d00` | `sub_08086A2C` | 208 |
| `fb4161f` | `sub_0808A880` | 140 |
