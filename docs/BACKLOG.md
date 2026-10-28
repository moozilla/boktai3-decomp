# Research and development backlog

Recorded from the user's October 12, 2026 discussion. These are the full set
of proposed directions, not a blanket authorization to implement all of them.
The initial authorized solo pass covered compact diffs, candidate tracking,
worker guidance and one subsystem. The user subsequently authorized the
two-worker pilot described at the end of this backlog.

## Compiler provenance and recipe audit

The user questions whether per-file compilers/flags conceal an incorrect
compiler choice. Investigate the provenance of pret's reconstructed historical
GCC toolchain and whether its particular version is justified for Boktai 3,
rather than assuming the Pokémon choice applies. Development/release dates
and contemporary compiler versions could constrain candidates. Compare that
research's likely value with continuing the demonstrated matching recipe.
Separate game code from statically linked SDK/runtime libraries, which can
legitimately use different compilers. Explain why near-complete library matches
still have residues despite known source and flags.

Next experiment: recipe inventory and controlled fingerprints across exact
controls plus a related blocked family. Thousands of exact matches establish
compatibility, not historical executable provenance. The October 9 solo notes
already record an unsuccessful prologue-bugfix probe; do not repeat it blindly.

## Work selection, architecture, and useful source

The user questions scattered leaf-function matching versus following startup,
the main loop and its calls into subsystems. Examine whether architecture-led
work recovers shared structs and APIs and improves throughput. DeepWiki's
analysis at https://deepwiki.com/moozilla/boktai3-decomp is a source of hypotheses
to check against code, not authoritative evidence. Recover subsystem maps and
function roles now rather than postponing everything to cleanup. Organize
related cohorts without requiring immediate physical TU consolidation.

The desired source should support translation, ROM hacks and possible ports,
not only a matching percentage. Explain what functions do through evidence-
backed names and comments. Adapt useful naming conventions from KNIDL, the
previously reviewed Kirby: Nightmare in Dream Land project. Keep observed
mechanics, semantic interpretation and uncertainty distinct. Coverage tags
mean observed execution, not exclusive subsystem ownership.

## Data policy and a browser-based game studio

Establish a project data policy informed by KNIDL's data and naming documents.
Progressively annotate the entire data region: ranges, formats, structures,
references, assets, consumers and confidence. Preserve unknown fields and raw
bytes; require byte-exact decode/re-encode where applicable. Keep source and
schemas separate from extracted copyrighted assets.

Long-term user vision: a polished in-browser full-game studio for asset viewing,
editing and mod development. Users supply a local ROM so assets are extracted
locally. Explore graphics, palettes, maps, audio, text, scripts and structured
records as formats become understood. Start with a complete inspect/edit/rebuild
slice rather than waiting for all data to be mapped; verify changes in-game.
This is a separate product goal, not part of the current matching target.

## Engine documentation as HTML with explainer graphics

Render `docs/ENGINE.md` and `docs/GAME_LAYOUT.md` as a browsable HTML site with
explainer graphics: the task-list and frame loop, the script call path, the memory
map, and the grand layout from ROM regions to subsystems. Build any artwork from
locally extracted assets (tiles, palettes, maps) under the ignored `build/` tree,
so no ROM-derived image is committed. Keep the Markdown as the source of truth
and generate the site from it. Build it alongside the planned data viewer and
editor, reusing its extraction pipeline and asset loaders. Every figure should
link to the function or unit it depicts, and every *(guess)* in the text should
stay marked in the figure. Start after the layout and engine documents settle.

## Programmatic pattern knowledge and broad draft generation

The user wants systematic recognition of disassembly building blocks, repeated
switches, source idioms, macros or helpers that consistently reproduce codegen.
Scattered worker notes are difficult to retrieve; one giant lessons file is
also inefficient. Build a machine-searchable corpus of matched examples,
compiler recipes, applicability, failures and reproducible probes. Investigate
structural families beyond current exact/loose clones, which often produce
only a handful of siblings. Prefer useful types/helpers over opaque matching
macros. Existing permuters are one component, not the complete strategy.

Explore scanning the entire ROM for switches and other repeated structures,
then generating potentially thousands of partial starting points. Separate
cheap structural inventories from useful C drafts. Sample and measure downstream
value before scaling; draft count alone is not progress.

## Partial matches, preservation and evaluation

Failed attempts, especially from weaker workers, should retain useful work for
the next agent. Quantify size, instruction, register and order differences;
store source, exact recipe, source hash, target extent, validity concerns,
failed hypotheses and next steps. Keep alternate candidates when they expose
different useful structure. Existing WIP branches supply durable retention,
but evaluation and retrieval need improvement. A register-normalized score
must not hide branches, constants, memory accesses or semantic problems.
Keep exact accepted progress separate from candidate similarity. Matching
chunks can change when the surrounding function's allocation changes.

## Compact feedback and deterministic workflow automation

Collapse matching disassembly into mismatch hunks with context. Keep full
functions available for first inspection and difficult allocation questions.
Align insertions/deletions rather than showing a positional cascade. Provide
machine-readable summaries and explicit compiler/link errors, preserving full
artifacts locally. Small functions may need little collapsing; large ones do.

Replace five or six routine model/tool exchanges with deterministic commands:
check existing matches and boundaries, retrieve candidates/siblings, prepare
context, compile, compare, checkpoint and validate. Preserve mandatory full
builds before commits and reviewed batched integration; reduce narration and
transcripts around routine success. Measure token savings and useful outcomes,
not merely command count. Do not turn semantic judgment into blind automation.

## Prompt caching, model economics and solo reasoning

Stable shared worker instructions should precede variable assignments; avoid
changing prefixes unnecessarily. Audit actual cache behavior and input/cached/
output usage, including the orchestrator. Do not assume identical visible
prefixes guarantee cache hits or translate directly to subscription savings.
Avoid overinvesting in cache tuning. Cheaper workflows could later support
more workers; this session deliberately tests one strong Astra agent instead
of many weaker subagents. Compare useful accepted bytes and reusable knowledge
against total effort; model labels and raw function counts are insufficient.

## Further decompilation research

Review more projects, particularly agent-driven matching decomps and firsthand
write-ups. Identify successful strategies and evidence of efficiency. The user
suggested low commit counts as one possible signal; account for squashing,
one-function policies, source imports, project size and completion definitions
before comparing. Prefer measured time/tokens and reproducible methods when
available. KNIDL is the existing reference, not the only one to consider.

Initial leads (not validated recommendations):
- https://gambiconf.substack.com/p/starting-a-decompilation-project
- https://github.com/JRickey/SLaDe-AGBCC

Use bounded questions: type recovery, compiler diagnostics, reusable corpus
retrieval, candidate retention, automation and actual costs. Avoid an open-ended
survey replacing practical experiments.

## October 12 external-research triage and execution order

The user supplied a comparative research memo covering Mizuchi, Snowboard Kids 2,
Klonoa, KNIDL, an FPS reconstruction, and several tools. Its statistics are
reported research, not independently reproduced Boktai benchmarks. The useful
methods below are adopted as bounded experiments; none changes acceptance.

1. **Now: measured matching campaign.** Two isolated Sol 6.1/high workers,
   twenty minutes each, explicitly authorized after the earlier solo trial.
   Use a small production family and one diagnostic family. Record audited
   bodies separately from inventory spans, seed effort, sibling opportunity,
   exact new bytes, duplicate rewrites, failed checks, wall time and coordinator
   effort. Stop on a plateau with no new hypothesis; ten attempts is a budget
   rule, not proof of impossibility. See `campaigns/2026-10-12.json`.
2. **Small ranked campaign manifests before a new scheduler.** Verified:
   worklist.py filters/emits assembly order; it does not sort by difficulty,
   and --chunks divides counts rather than effort/bytes. Rank a short queue
   by audited byte opportunity, exact sibling/source evidence, shared layouts,
   seed cost and prior failures. Large spans and high similarity alone do not
   imply good return. FDB4's 488-byte tail is not part of its 300-byte draft.
3. **Next only if needed: asmlift draft pilot.** Compare assembly-only, current
   Ghidra and asmlift drafts on about twelve fixed targets (ordinary logic,
   arrays/structs, a switch, hard family). Hide exact source from control arms,
   exclude controls from progress, and hold recipe, context, model and budgets
   fixed. Measure valid compiles, direct exacts, tokens to first exact and final
   accepted bytes including failures/integration. Stop adapter work if setup
   exceeds demonstrated savings. Keep the existing checker as the oracle.
4. **Bounded compiler diagnostics.** Reproduce the retained candidate and
   symbol baseline; inspect the first differing operation. At most three
   distinct hypotheses about width, aliasing, lifetime, temporary placement,
   branch structure or TU context, then a short search only with a useful
   gradient. A diagnostic compiler may add traces, never alter production
   codegen; compare with the untouched compiler on controls and the target.
5. **Later: guided-search comparison.** On two structurally valid candidates,
   compare equal CPU-time decomp-permuter versus explicit alternatives or
   Transmuter. Retain variants and review validity; zero normalized score is
   not acceptance. Stop random walks without a useful gradient. Do not replace
   the current permuter merely because a new tool exists.
6. **Compact task/result contracts.** Assign address/endpoints, accepted base,
   compiler identity/flags, baseline ELF, exact sibling, relevant types, best
   candidate hash, failed hypotheses, allowed files, goal and stop condition.
   Return source hash, disposition, emitted/reviewed sizes, first difference,
   category, full-build evidence, new bytes, inherited help and one next test.
   Fingerprints must prevent a previous check from authorizing edited source.
7. **Integration receipts and focused negative controls.** ROM-free progress
   CI is source-membership accounting, not a ROM rebuild. A receipt should bind
   source commit/tree, compiler identity, private reference hash and full-build
   result. Extend existing tests only for gaps: stale symbol snapshots, missing
   Thumb bits, wrong ARM/Thumb recipe, changed source after checking, body/tail
   credit and wrong branch targets. Truncated-body and error-classification
   controls now exist. No ROM in public CI and no reviewer fleet per function.
8. **Cheap coordination and honest economics.** Automate target claiming,
   unchanged-result handling and ledger updates only where repeated friction
   is measured. Include failures, source reuse and coordinator usage. Keep API
   dollars, price proxies and rounded subscription-meter readings separate.
   Distinguish cost, elapsed-time and concurrency objectives. A new model
   comparison needs equal packets/settings/budgets; the old six-target pilot
   was confounded. Identical prompts alone do not prove cache savings.
9. **Runtime work must answer a matching question during matching tranches.**
   Use existing replay/watchpoints to resolve a particular callback, field or
   table. Broad playthrough/naming/editor work remains worthwhile with its own
   budget, not silently charged to exact-byte production.

Research references retained for the experiments (not installations):
- Mizuchi methodology: https://gambiconf.substack.com/p/can-llms-really-do-matching-decompilation
- Snowboard Kids long tail: https://blog.chrislewis.au/the-long-tail-of-llm-assisted-decompilation/
- Completion/source-policy comparison: https://blog.chrislewis.au/snowboard-kids-2-is-100-decompiled/
- Small queue/control mechanisms: https://github.com/cdlewis/nigel
- Klonoa production lessons: https://gambiconf.substack.com/p/starting-a-decompilation-project
- Draft transcription: https://github.com/macabeus/asmlift
- Guided search: https://github.com/macabeus/transmuter
- Diagnostic compiler reference: https://github.com/Dream-Atelier/agbcc
- Source fingerprints/plateau classification: https://github.com/Muhtasham/decomp-goal-harness
- Oracle/relocation lessons: https://momo5502.com/posts/2026-10-09-game-decompilation/

Do not import inline-assembly exceptions, verifier exclusions,
compiler forks that manufacture matches, or partial-credit commit policies.
Do not extrapolate Mizuchi's reported per-function costs to this depleted easy
frontier. Similarity leaderboards and Linux x86 re-executability results are
not agbcc exact-success benchmarks. A playable game, a source-present function,
and clean portable source are different deliverables from exact matching.

### Pilot outcome and next selection

The completed October 12 pilot added three exact functions / 1,236 bytes from
a fresh snapshot/reset family; the retained register-residue diagnostic added
zero. Both stopped within budget. Prefer another small fresh-family campaign
before the asmlift experiment, a scheduler, or increased concurrency.
`candidate.py --diff` now removes the duplicate check workers encountered.
The account meter rose three percentage points including coordination; this
is not proof of economical scale. See the campaign receipt for source-level
results and separate model usage records.

Register pinning is now explicitly authorized (October 12): use marked local
register declarations for exact matching, with cleanup deferred. Track their
source completeness separately through decomp.dev progress categories.

## Sprite palettes: unreached sprites (backlogged October 17, 2026)

`tools/assets/paltrace.py` resolves 303 of the 419 palette calls; 108 sprites
are fully accounted for (34 default only). 171 sprites are neither traced to
a load nor loaded in any scene of the palette survey: probably bosses,
events and menus the survey doesn't reach, or unused. The next step would be
debug scenarios (`tools/mgba/scenario.lua`, late-game save, `call` of the
boss and event spawner procedures) with `PALETTE_LOG=1`, which now logs
every sprite load with its caller as well (`sprite_loads.tsv`), then
`bigpalettes.py write`. Open static cases: sprite `848F` passed through a
descriptor in four functions, the floor switch's `T=` sprites `17C7`-`17CB`
(a table), and `-t` values the GCL computes for `08055268`. Backlogged in
favor of Studio and tooling work; the scenarios would also give runtime
coverage of boss code for the decomp if picked up.

## Data tracking on decomp.dev: ROM data and Studio structures (backlogged October 26, 2026)

decomp.dev shows objdiff's data measures. Since October 26 they count RAM
objects (`symbols/ram.csv`, [RAM.md](RAM.md)). ROM data is not counted: the
tables, scripts, graphics and maps the Studio already decodes (`tools/studio/`,
[ASSETS.md](ASSETS.md), `symbols/data.csv`). A ROM-data measure would need an
inventory of `gen/data.s` ranges with a status per range: raw bytes, a
structure with a decoder, or C/asset source that rebuilds identically. Then add
those bytes to `total_data`/`matched_data` next to RAM, or report them as a
separate category. Backlogged with the RAM accounting landed.

Researched on October 27: [DATA_TRACKING.md](DATA_TRACKING.md) has the objdiff
semantics, the current ROM data numbers and the plan; the report (ROM data as
the headline data measure, categories by kind) landed the same day.
