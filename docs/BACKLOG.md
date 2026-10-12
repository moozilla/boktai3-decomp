# Research and development backlog

Recorded from the user's October 12, 2026 discussion. These are the full set
of proposed directions, not a blanket authorization to implement all of them.
The current authorized solo session starts with compact diffs, candidate
tracking, worker guidance, then one promising subsystem. No delegated workers.

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
