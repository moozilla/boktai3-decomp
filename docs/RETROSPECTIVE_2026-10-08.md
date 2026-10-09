# Boktai 3 matching run retrospective

This document first audits the earlier sustained run, then the later matching
follow-up and bounded draft rescue in the final sections. The final source
state is 13.377% code matched; the 20% target remains unmet.

The sustained run delivered too little matched code for the allowance consumed.
From `e2546f6` to `5ff44e3`, matched code rose from **9.302% to 12.282%** while
weekly allowance fell from **73% to 33% remaining**. That is 40 allowance
percentage points for 2.980 code percentage points: **71,970 additional bytes
and 422 functions**. The requested 20% code target was not reached. New work
stopped at 35%; finishing existing assignments and integration consumed the
remaining two points. These are account-meter observations, not an invoice
attributing every point exclusively to this project.

The main orchestration mistake was failing to budget the main thread itself.
It ran Astra at extra-high reasoning through hundreds of turns of matching,
tooling, coordination, documentation, and integration. Local logs now make it
possible to quantify that much better than the original run reports did.
The second mistake was allowing several objectives to compete with matching
without separate budgets or an early enough return-on-effort check.

The user's criticism of side work is partly supported. Some infrastructure
produced no native-byte gain during this run. However, library research was
the largest source of matching progress, so cutting all research would target
the wrong work. The problem was the allocation and execution cost of the work,
not simply whether it was called research or matching.

| Scope | Starting matched bytes | Ending matched bytes | New functions | Gain |
|---|---:|---:|---:|---:|
| Entire local session, `e9d2c21` through `5ff44e3` | 220,400 | 296,658 | 480 | 76,258 bytes / 3.157 percentage points |
| Sustained allowance-limited run, `e2546f6` through `5ff44e3` | 224,688 | 296,658 | 422 | 71,970 bytes / 2.980 percentage points |

The 73% allowance observation belongs to the second row. Setup, the pilot,
initial CI/context work, and production round 1 preceded it; their allowance
must not be silently charged to that 40-point interval. All code percentages
use the fixed 2,415,354-byte code-region denominator. These are matched-byte
percentages, not emulator execution coverage or whole-game completion.

**What the usage audit actually establishes.** The desktop's local session
JSONL files contain per-response `token_usage_record` entries, even though the
coordination tools did not expose a per-worker billing meter. The audit selects
this root thread and its direct children, deduplicates response IDs, and sums
records from 2026-10-09 02:07:01.374 UTC to before 04:20 UTC. The latter cutoff
includes the closing report and excludes this retrospective. Reasoning tokens
are already part of output tokens and are not added twice. Worker names persist
across rounds: `round1_sol`, for example, includes its later assignments.

The table uses a **standard-speed credit-equivalent proxy**, not actual billed
credits or a conversion to subscription allowance. Its rates per million
uncached input / cached input / output tokens are Astra 250 / 25 / 1,250;
Sol 6.1 50 / 2.5 / 250; Luna 2.5 / 0.25 / 12.5. Published credit prices do not
alone determine included-plan usage. Source: [OpenAI pricing](https://learn.chatgpt.com/docs/pricing),
checked October 8 Pacific. No historical speed multiplier is assumed.

| Worker | Recorded model and reasoning | Uncached input | Cached input | Output | Credit-equivalent proxy |
|---|---|---:|---:|---:|---:|
| Main thread | Astra / xhigh | 865,605 | 64,015,360 | 211,343 | 2,080.96 |
| Luna A | Luna / medium | 539,913 | 42,797,312 | 105,220 | 13.36 |
| Luna B | Luna / medium | 812,124 | 47,313,920 | 152,975 | 15.77 |
| General Sol worker | Sol 6.1 / low | 766,807 | 59,402,368 | 192,108 | 234.87 |
| Requested process review | Astra / high | 122,879 | 2,490,496 | 13,058 | 109.30 |
| MGS worker | Sol 6.1 / high | 525,796 | 31,225,088 | 159,452 | 144.22 |
| Family worker | Sol 6.1 / high | 424,449 | 34,327,040 | 115,132 | 135.82 |

For each row, the calculation is `(uncached × input rate + cached × cache rate
+ output × output rate) / 1,000,000`. Total: approximately 2,734.32 proxy credits.
The main thread contributes **76.1%**, all Sol workers **18.8%**, the separate
Astra review **4.0%**, and both Luna production workers **1.1%**. Including the
earlier setup/pilot period, the main thread contributes 79.6% of the full-session
proxy. These shares suggest where to change allocation; they are not claims
that Astra consumed exactly 76.1% of the user's 40 allowance points.

The main thread made 430 model responses during the sustained interval. Its
median input was about 156,000 tokens per response, with five compactions.
Cached context still has a nonzero price in this proxy: the main thread's
cached input alone contributes 58.5% of the entire run's proxy. This repeated
context contains instructions, tool definitions, history, and tool results;
it is not 64 million tokens of unique new information. The exact savings from
a shorter context or a different model cannot be inferred without rerunning
the work, but this is a stronger explanation of the cost pattern than blaming
the number of Luna workers.

The earlier 4.92M-token goal-counter figure was not a billing breakdown. It
should not have been presented as if it explained allowance consumption.
The auditable categories above are more informative. The local aggregate is
saved at `build/retrospective/usage-audit.json`; raw session logs stay outside
the repository and are not published.

**Where matching progress came from.** The following table reconciles the
sustained run's library gains with the progress reports. It counts incremental
progress spans, including the existing accounting policy for literal pools
and alignment, rather than mixing them with total library object sizes.

| Work | New functions | Incremental code bytes |
|---|---:|---:|
| Floating-point libgcc | 40 | 5,864 |
| fdlibm math routines | 22 | 11,584 |
| RTC routines | 14 | 1,784 |
| memcpy and memset | 2 | 178 |
| EEPROM routines | 5 | 808 |
| RFU SDK | 91 | 16,272 |
| Library and SDK subtotal | **174** | **36,490** |
| Other matching, including families, MGS, and solar code | **248** | **35,480** |
| Total | **422** | **71,970** |

Library/source identification delivered **50.7% of the byte gain**. RFU's
145-function final unit contains 54 previously matched functions; claiming
145 new matches would double-count them. The seven MGS rounds yielded 50
matches / 7,024 emitted bytes. MGS is included in the remainder row, not an
additional gain. Emitted bytes and progress spans differ, so those figures
should not be added directly.

**What I should have done differently.**

1. **Account for expensive orchestration before optimizing worker choice.**
   I debated Luna versus Sol while using Astra/xhigh for routine integration,
   polling, notes, and source inspection. Astra also produced valuable large
   matches, so its entire share is not overhead. Nevertheless, I should have
   surfaced the main-thread cost and proposed Sol for routine orchestration,
   reserving Astra for bounded investigations with a concrete candidate or
   source hypothesis. A model selected for hard reasoning need not also carry
   every administrative step in a large, persistent context.

2. **Separate throughput from cost efficiency.** Luna A's rounds 2 and 3
   produced 328 and 788 bytes in roughly 19 minutes each. Luna B's round 3
   produced 264 bytes in 20 minutes, followed by a nine-minute round with zero
   matches. Sol's contemporaneous rounds 2 and 3 produced 3,736 and 6,736 bytes
   in roughly 24 minutes each. Those heterogeneous tasks justified reallocating
   scarce worker slots sooner for throughput. They did not establish that
   Luna was allowance-inefficient: its token-price proxy was very small.
   The user had to prompt the change instead of receiving an earlier,
   evidence-based allocation decision.

3. **Run an honest model comparison.** The six-target pilot reported success
   and wall time, but did not establish a Pareto frontier. The local logs now
   reveal another confounder: Sol used low reasoning; Luna and Astra used
   medium. The protocol's original claim of a common reasoning setting was
   wrong and has been corrected. Unequal persistence, different production
   targets, inherited drafts, and source reuse further prevent a clean ranking.
   Even the pilot's reconstructed proxy favors Luna on cost while Sol is
   faster and Astra solves the extra target. None of that proves which model
   would beat Claude on an equally budgeted production workload.

4. **Give side work an explicit budget and payoff criterion.** Local setup and
   progress tracking were requested deliverables. Much of that work preceded
   the 40-point interval, so it cannot explain all of the sustained-run cost.
   Compiler selection, cache fixes, and mixed ARM/Thumb splitting directly
   enabled large source matches. The script probes, context attribution, menu
   labeling, and architecture notes had value, but mostly no measured native
   matching payoff during the run. I should have reported those separately
   and capped further investment once the priority became reaching 20%.
   There is no trustworthy time ledger proving that a majority of active time
   went to side work, and I should not invent one.

5. **Make source reuse and family expansion the first search strategy.** The
   repository already recommended library signatures and clone seeds. Both
   worked. Repeated local source/register experiments on isolated near-matches
   had a lower observed return, including several zero-yield MGS follow-ups
   and an unresolved large motion-family seed. Hard attempts were appropriate
   to the request, but should have been bounded by expected family byte gain,
   explicit new hypotheses, and a reusable failure record.

6. **Keep validation, reduce the model work around it.** Exact checks,
   complete-ROM builds, and the data-shift regression are acceptance evidence,
   not optional polish. Running a compiler or emulator is not itself an LLM
   reasoning step. The expensive part can be repeated inspection, long tool
   transcripts, and narration around unchanged results. Retain the repository's
   required builds and one combined integration regression, reuse their logs,
   and return compact results unless something fails. The user's instruction
   to stop rechecking the live progress site was appropriate.

**What can be said about Claude.** The inherited handoff reports Sonnet-class
workers producing 80–135 matches per round at 130–340k tokens. That is stronger
function throughput than most of these individual worker rounds, and the
user's report of better value should be taken seriously. The historical source
already contained 4,145 matches before this session; this session added 480.
However, the records here do not provide equivalent Claude input/cache/output
usage or its plan allowance denominator. Earlier work also exhausted many
short functions and large clone families. Those are comparison limitations,
not an excuse or evidence that Claude was necessarily given easier work.
Claude may be a better matcher or better value for this project. This run did
not conduct the experiment needed to answer that.

For the next matching run, the proposed operating policy is a small allowance
tranche with a before/after byte and usage report, a Sol orchestrator with
compact task context, source/library and family queues prepared before worker
launch, and one bounded Astra escalation at a time. Record model, actual
reasoning setting, token categories, checks, new bytes, and inherited assistance
for every round. Judge production by accepted bytes and allowance together;
do not replace cost accounting with functions per minute. Preserve the MGS
assignment when requested, but give it a bounded slice of the matching budget.
Research should state the match or contextual question it intends to unlock.
No new matching workers were started to prepare this retrospective.

Evidence: [model and round results](MODEL_BENCHMARK.md), the per-round files in
[`notes/`](../notes/), the `PROGRESS.md` snapshots at the commits above,
[library](LIBGCC.md), [math](LIBM.md), [RFU](RFU.md), and
[script-tracing](SCRIPT_TRACING.md) reports. The separate
[devlog](DEVLOG_2026-10-08.md) describes the technical results without treating
infrastructure or research as extra matched-code coverage.

## Follow-up: the 20% usage floor

The subsequent authorized matching push began at `5ff44e3`: 296,658 bytes
(12.282%) and 4,625 matched functions, with 32% account allowance remaining.
It ended at `8c2c818`: **319,366 bytes (13.222%), 4,741 matched functions**, and
20% allowance remaining. Four reviewed batches added **22,708 progress bytes /
116 functions**. That is 0.940 code percentage points for 12 account-allowance
points, about 1,892 bytes per allowance point. These are account observations,
not per-model billing or a controlled comparison. The earlier per-response
cost-proxy audit above does not cover this follow-up.

The 20% code target was missed by **163,705 bytes**. Sol 6.1 at high reasoning
was the worker default, so the pilot's low-reasoning setting cannot explain
this follow-up's result. Bounded Astra-low work helped one large math draft
that Sol subsequently finished; other blocked attempts added no matches. The
last Astra attempt stopped immediately when its usage check reached the floor.
No matching work resumed after that gate; the user's final instruction was to
finish and push documentation, without starting new agents.

The earlier diagnosis was not fully converted into a better operating policy.
The run still spent too many model turns on coordination, isolated near-matches
and tiny late rounds. After the user objected to test overhead, broad tool tests
were dropped for the final two C/metadata batches, and drafts were capped.
However, integrating batches of 2,688 and 648 bytes still repeated the required
build/shift cycle and its surrounding model work for little gain. Validation
proved accepted code; it did not make those small batches efficient. We do not
have a time ledger or new model-cost audit that attributes the loss to tests.

The last supposedly larger worklist target also exposed a selection problem:
0811FDB4's 788-byte progress span contains a 300-byte body and a separate
488-byte retained tail. Low branch counts and large inventory spans alone are
poor priority signals. Inspect actual body size, likely family expansion and
source evidence before assigning a supposedly high-yield candidate.

For the next authorized tranche, prepare a short queue of actual large bodies,
source matches or seed families with explicit projected byte gains. Set a
tranche allowance cap and a minimum accepted-byte yield checkpoint before
starting. Stop low-yield families at that checkpoint; do not keep retrying them
through automatic continuations. Hold small accepted queues for one combined
integration at tranche end, while retaining the per-function exact checks and
complete build required before source commits. Reuse compact validation logs
instead of generating repeated narration around unchanged results. Record the
orchestrator's token use alongside worker use before drawing cost conclusions.

All accepted source is merged through PRs #9–#12; all workers are finished.
The final full ROM and 35 shifted-data screenshot comparisons pass. Safe
nonmatching drafts remain local and ignored, including the last Sol FDB4 draft.
No unfinished source was promoted to make the closeout look successful. The
[handoff](HANDOFF.md) gives current counts, limits and candidate locations;
the [follow-up report](PUSH20_2026-10-08.md) preserves batch accounting.


## Existing-draft rescue: missed work and family efficiency

The first closeout was incomplete: accepted reviewed batches were merged,
but ignored WIP had not been exhaustively inventoried. Two exact candidates
were overlooked, and many clone drafts still contained stale constants or
layout values that were inexpensive to repair. The bounded rescue checked
113 previously unmatched targets plus 68 distinct alternates and finished
**48 functions / 3,724 bytes**. It advances 13.222% to 13.377%, with the
account meter at 19% remaining during integration after a 20% starting reading.
Rounded account observations cannot establish per-model costs.

The strong result within this rescue was broad family repair. Literal-pool
values and compiler-encoded shifted constants had escaped earlier clone
porting. Fixing the family parameters yielded more exacts than prolonged
single-instruction/register tuning. The existing Astra-low worker's three
bounded resumes produced no matches. Six capped permuter searches also
produced no accepted exact source; a low-score candidate with an early return
and unreachable logic was rejected. No new agents or function targets began.

The audit also exposed why inventory spans need review before credit: the
RTC time-write draft emits 156 bytes, while its previous span included a
separate 308-byte unfinished routine. The final boundary preserves zero
credit for that routine. Source review and the complete ROM check are still
necessary even after a standalone exact check.

Partial source retention has a useful role in preserving algorithms, layouts,
and failed hypotheses across sessions. The present rule forbids committing
nonmatching C, so this closeout commits metadata only. A separate candidate
store, excluded from production and exact progress, is a concrete future
option for the user to approve. Matching source chunks are not independently
stable: completing the function can change register allocation and scheduling.
A similarity percentage must not be described as semantic correctness.

All 65 unresolved targets are explicitly inventoried rather than called
finished. Local ignored WIP survives while the checkouts remain; a fresh clone
does not recover it. This distinction belongs in every future closeout.

The final integration needed a narrow splitter repair: exact hidden bodies
spanned multiple incbin fragments around symbolized literals. Reviewed
replacement endpoints for 08232F1C and 08248E70 consume only those bodies,
leaving neighboring assembly intact. Five focused boundary/compiler tests
pass, including a regression that preserves a fragment crossing the endpoint.
This required tooling check is separate from the broad suite omitted above.

Final accepted-source head: `82d700d`. Both the original and restored complete
ROM builds print `build/boktai3.gba: OK`; all 35 shifted-data screenshots pass
(intro 16 / save 7 / menus 12). All exacts are separately committed for one
combined integration. The 65 unresolved candidates remain local and ignored.
A source-only local archive preserves 519 candidate files, including duplicate
and already-matched drafts; it is not available in a GitHub clone.
