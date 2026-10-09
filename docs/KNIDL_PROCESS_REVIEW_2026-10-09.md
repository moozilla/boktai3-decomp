# KNIDL workflow review and implications for Boktai 3

Reviewed October 9, 2026. Reference snapshot:
[`overjt/knidl@628f4d4`](https://github.com/overjt/knidl/tree/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b).
This is a repository/documentation review, not an independent ROM build or a
controlled model-efficiency benchmark. No matching work or new agents were
started for this review.

## Findings

The strongest transferable methods are dependency-aware family assignments,
shared seeds and declarations before fan-out, compiler diagnostics for hard
allocation differences, and durable checkpoints. Our basic exact-check/build
pipeline already follows much of theirs. The main gap is how we select,
group and finish work, rather than the choice to verify functions individually.

Evidence reviewed: the 972-commit reachable history index, six representative
commit changes, README, AGENTS, the module map and its algorithm, history,
decompilation loop, diffing, splitting, compiler-validation and prior-art
research, tooling-pipeline research, data convention, audit, progress-report
documentation and generator, fnmatch, Makefile, and the relevant lessons plus
their supersession index. The lessons file is a large campaign archive; this
review follows its summary and relevant numbered sections rather than treating
all historical advice as current. Local reference downloads remain ignored.

## Compare scope and completion first

| Measure | KNIDL snapshot | Boktai 3 after PR #14 |
|---|---:|---:|
| Known functions | 5,348 | 11,081 |
| Tracked code bytes | 851,196 | 2,415,354 |
| C code bytes | 847,028 | 323,090 |
| Remaining code | No compiler-generated game code left | 2,092,264 bytes outside exact C |
| Public code headline | 100%, including assembly by design | 13.377% exact C |

KNIDL is about one-third our code size. Its 4,168 assembly bytes are deliberately
retained, documented and counted as complete source by its public reporter;
its local C percentage is 99.5103%. That small distinction does not explain
away its achievement. It does mean that its badge and our exact-C target use
different conventions. We should keep our denominator and target stable.
[README](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/README.md),
[reporting convention](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/decomp-dev.md).

Its reporting units group functions into modules or early/SDK ranges; ours
maps one function to one unit. That explains large *unit* count differences.
Boktai 3 also has about twice as many known function entries. That difference
comes from the binary inventories, not from putting functions in separate C
files. These are project census counts, not an independent boundary audit.

## What the development history shows

The reachable history runs from August 20 to October 5, 2026. Initial work
established the toolchain, SDK/source references, symbols and splitting.
August 25's compiler correction was followed by bulk module landings in late
August and early September. Remaining difficult functions occupied later
campaigns; the September 27 history records game-code completion and a
natural-C cleanup. Much subsequent work concerns naming, data, relocation,
testing and the final audit, rather than adding game-code bytes.

The project explicitly describes agent assignments and fan-out, but also
requires the human owner to approve merges. Commit authorship is usually the
owner's identity, and some messages contain AI coauthor attribution. These
records support an agent-heavy implementation with human coordination; they
do not establish autonomous completion, total tokens, cost or model-specific
efficiency. We should not compare their multiweek history with our single-day
allowance as if they were equal budgets.
[History](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/history.md),
[owner/agent workflow](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/AGENTS.md#git--pr-workflow-mandatory-for-agents).

## Methods with demonstrated usefulness

### 1. Plan modules from dependencies and dispatch families

Their module mapper partitions the bulk region into 37 modules of roughly
12–32 KiB. It uses nearby call traffic, dispatch-table anchors and literal
references. Pointer-table edges are anchors rather than graph unions, avoiding
a giant component of unrelated code. Smaller batches sit inside the modules.
Shared actor APIs precede 282 KiB of repeated behaviour-bank work; more coupled
engine code comes later.
[Module map and algorithm](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/analysis/module-map.md).

For us, the useful change is to commission a bounded related cohort with a
projected byte yield, not another arbitrary address sweep or a collection of
unrelated short functions. We already have scene context, script/actor
registries and matched family seeds; those should feed the work queue. We do
not need to recreate their entire mapping tool before using this principle.
Their campaigns also removed phantom call targets and repaired pool/body
boundaries. Our RTC rescue found a similar span problem. Check actual body
size and boundary evidence before treating a large inventory span as a
high-yield assignment; this is not a reason to change the denominator to make
progress look better.
[Splitting evidence](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/splitting.md).

### 2. Seed helpers and representative source before parallel work

They often matched cross-family helpers and representatives first, then
assigned families with shared type/signature notes. Lesson 4.98 reports 21
seed functions followed by four agents finishing 58 more in about 35 minutes;
two remaining functions needed handovers. Other history entries describe
seed sets of 13–59 functions. These are reported examples, not a cost study.
[Lessons 4.98 and 4.76](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/lessons-learned.md).

Our assignments should include an exact sibling, relevant offsets/strides,
shared declarations, recipe and the family's differences. The draft rescue's
48 matches from only 3,724 bytes demonstrates the immediate value of repairing
family parameters; it also shows why tiny families alone cannot deliver the
remaining 159,981 bytes required for 20%. Choose families with substantial
remaining bodies, not just many wrappers. Per-function commits can remain.

### 3. Use transcription and source reuse to reduce drafting work

Their workflow uses related-project source for SDK-shaped code and m2c as a
first-pass transcriber for logic-heavy modules. Drafts supply data flow and
argument order and are then verified through a fast standalone byte checker.
The final build establishes integration, including placement and literal pools.
[Decompilation loop](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/decomp-loop.md),
[prior-art research](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/research/prior-art.md).

We already do this successfully for SDK libraries and have Ghidra drafts.
The transfer opportunity is systematic family reuse, not assuming an m2c
installation will solve our remaining code. KNIDL also has a particularly
close Kirby reference project; our MGS similarities are useful evidence but
are not interchangeable game source.

### 4. Diagnose allocation differences instead of guessing endlessly

They used GCC allocation/RTL dumps and, for selected hard cases, instrumented
compiler internals. A later cleanup replaced pins/levers in 130 of 133
functions with plainer matching source. Wrong widths, volatility, repeated
reads and control-flow structure explained many residues.
[Natural-C history](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/history.md),
[actual removal of ten pins and six levers](https://github.com/overjt/knidl/commit/0a388b61e2cc05d1c47cca4ad73e103c18922760).

Our worker brief currently recommends the permuter for cases where only
registers differ. Their experience challenges making that the default. We
also have recent failed searches and a rejected invalid low-score candidate.
For a valuable retained near-match, first revisit declared types and its
plain matched sibling, then use compiler dumps to test one allocation
hypothesis. Reserve permutation for source-shape searches with a meaningful
scoring gradient. Do not copy their historical pins or sanctioned exceptions:
our current matching standards prohibit those tactics.

They also found standalone and combined-TU matches could differ because of
compiler pool-label state (lesson 4.79). Our one-function files work for
thousands of matches, but a valuable stubborn residue may warrant a controlled
compilation-context probe. This is a hypothesis to investigate, not evidence
for merging all our files or changing function counts.
[Compilation-context lesson](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/lessons-learned.md).

### 5. Validate recipes, rather than importing compiler folklore

They initially assigned some game files to old_agbcc, then showed that
`-fprologue-bugfix` explained the apparent compiler distinction. The
[August 25 commit](https://github.com/overjt/knidl/commit/3fdd5b43b00528a2aac5c5852117b7fb1e1b971a)
removes those overrides. Their current
[Makefile](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/Makefile)
uses that flag for game code while retaining separate SDK recipes.

Our compiler's help exposes that flag and dump options, but this review did
not run a matching experiment. Our thousands of current matches are evidence
for the present recipe. Any flag trial should be limited to a relevant
blocked family, with exact controls; a global flag change is not justified.

### 6. Preserve partial work without pretending it is accepted code

The [August 25 WIP commit](https://github.com/overjt/knidl/commit/01860bb0af1178a8edbfbf176f3273e5ad78e9f1)
preserves a module draft whose size matched while register differences
remained. Their current
[report generator](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/tools/gen_report.py)
gives each function 100 or 0 according to accepted source-range membership;
aggregate `fuzzy_match_percent` is accepted-byte coverage, not measured
instruction similarity of unfinished candidates.

This supports our new WIP branch policy. For future triage, record source
hash/recipe, body versus tail size, size delta, mismatch category, known
validity problems and the next hypothesis. Fuzzy similarity can indicate
improvement or help find families. It cannot prove semantics, and a few
remaining register differences do not guarantee an inexpensive finish.
Matching chunks of a large function are useful evidence, but completing the
function can change allocation throughout it. Do not add partial credit to
the exact 20% target. Implementing new fuzzy reporting remains undecided.

## What to adopt later, and what not to copy now

Their consumer-proven data layouts and separation of proven pointers from
heuristics are sound, and largely agree with our symbolic/shiftable ROM
approach. Typed layouts can unlock matching families. They are also useful
for later editable data, but blanket data conversion and naming campaigns
are not priorities while we are at 13.377% exact code.
[Data convention](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/data.md).

Their final validation is extensive: exact builds, map ownership, relocation
census, frame comparisons and a documented exception audit. They do not
provide evidence for dropping exact builds. Our waste was repeatedly
integrating tiny batches and repeating surrounding orchestration, not proof
that verification is unnecessary. Keep fast per-function checks, consolidate
integration and run checks appropriate to the actual changes. Do not import
their entire completion-stage audit workload into every matching attempt.
[Audit](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/audit.md),
[diffing](https://github.com/overjt/knidl/blob/628f4d461227d7a9e53b02e9eac8f63f4db5cf8b/docs/diffing.md).

## Recommended next matching tranche

1. Start from the preserved 65-target inventory and existing family evidence.
   Select one or two cohorts by actual remaining body bytes and reuse
   potential. Keep a short queue of expensive outliers separately.
2. Supply exact representatives and shared declarations before fan-out.
   Set an expected-byte checkpoint and include the coordinator in the budget.
3. Diagnose one valuable allocation residue with compiler dumps; cap blind
   permutations and hand over the best checkpoint when that cap is reached.
4. Save improved candidates on WIP branches throughout the run. Integrate
   accepted exacts in one useful batch, retaining one-TU match commits.

This is a proposal for a future authorized tranche, not a newly started run.
The immediate changes are draft preservation and the main/WIP policy update.
