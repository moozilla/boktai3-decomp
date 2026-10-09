# MGS/GCL matching round 3

Sol 6.1 worker on `codex/mgs-gcl-round3` from `ace10e4`, same isolated checkout.

| New target | Bytes | Source variants | Match detail |
|---|---:|---:|---|
| `08225328` | 232 | 2 | Local constant 2 before pointer load reproduces store order |
| `0821A054` | 108 | 2 | Ordinary for-loop gives original next-list/counter allocation |
| `0821A218` | 108 | 1 | Resource-entry struct and retained-index count |
| `0821A490` | 88 | 2 | Reverse addition operands for base-relative result |

Final total: 4 functions, 536 native bytes. All new findings, including the
unmatched engine state machine, are in `docs/MGS_GCL_MATCHING.md`.

Untracked WIP: `082250FC` engine update after six source variants; ordinary
code eliminates a repeated saved procedure-ID check and a u8 helper emits extra
boolean materialization. `0821A284` entry insertion retains register/constant
ordering differences after three variants. `0821A184` allocator differs by a
counter/pointer register swap after three variants. A bounded 60-second,
two-worker permuter pass improved score 40 to 20 without a match; its best
source is `wip/sub_0821A184_permuted_20.c`. A fourth explicit-loop-limit
variant worsened scheduling. The improved candidate has the correct initial
counter/pointer allocation but swaps the saved mask and limit registers. Prior trap/vector/typed-load candidates were preserved.
No external C was copied, no names installed, no worker screenshots requested.

Validation: each of the four translation units passed its exact-match check.
The complete build printed `build/boktai3.gba: OK` before the matching commits.
The orchestrator owns the combined regression run; no additional worker
screenshot run was performed in this round.
