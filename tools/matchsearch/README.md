# Matching search: persistent hypotheses, compiler facts, ordinary C

Tags: #matching #compiler #tooling

`matchsearch` is a stateful workbench for an LLM acting as the search policy. It
reuses scope's fast compiler and adds a durable source graph, guarded AST moves,
machine-value constraints, region contracts, a diverse frontier, and persistent
compiler evidence. There is no model API, training job, or random permuter inside
the tool. The calling model decides which hypothesis to investigate next.

## Start here

The normal project setup and full baseline build must be complete. Build scope's
instrumented compiler with the current `tools/scope/agbcc.patch`; the patch now
distinguishes an initially empty reload rotation from forced group scanning.
Use the project's `build/venv/bin/python` and make the ARM binutils available on
`PATH`. Run from the repository root. No new runtime dependencies are needed.

```python
from pathlib import Path
from tools.matchsearch.session import Session

with Session.create(
    "build/matchsearch/player",
    repo=".",
    function="Player_GetSmoothedDirection",
    source_path="src/fn/sub_08141BAC.c",
) as session:
    print(session.status())

with Session("build/matchsearch/player") as session:
    slate = session.moves("root", families=["remove_pin"], limit=8)
    print(slate)
    result = session.expand(
        "root",
        ids=[move["id"] for move in slate["moves"]],
        families=["remove_pin"],
        hypothesis="Remove hard-register constraints to expose the ordinary allocator's value structure.",
        prediction={"register_pins": 0},
    )
    print(result)

    proposed_source = Path("build/my_candidate.c").read_text()
    node = session.propose(
        proposed_source,
        parent="root",
        label="capture-before-call",
        hypothesis="The ROM captures this value before the call, while the draft reloads it afterward.",
    )["id"]
    session.store.retain(node, "Preserve the call-boundary correction even if byte distance increases.")
    print(session.diagnose(node, names=["history", "sample"]))
    print(session.decision_delta("root", node))
    print(session.semantic(node, trials=16))
    print(session.verify(node))
    session.export(node, "build/candidate_to_review.c")
```

An `expand` operates on a bounded, policy-selected slate. Its hypothesis is
required. Predictions are recorded as intentions, not automatically certified.
It keeps every candidate, including compile failures and temporary regressions.
The frontier is a presentation of useful alternatives, not a deletion policy.
`retain` explicitly keeps an important hypothesis in that presentation.

Source strings are complete translation units. Ordinary source is the acceptance
authority: the session recompiles the exported source instead of trusting a
different preprocessed candidate. A matching node still needs the independent
per-unit check and the normal combined ROM build before integration.

## Commands for matching workers

Write a complete C unit in a file, then submit it directly. No JSON escaping is
needed. Nodes accept their hash prefix or a unique label.

```bash
build/venv/bin/python -m tools.matchsearch init build/matchsearch/player \
    src/fn/sub_08141BAC.c Player_GetSmoothedDirection
build/venv/bin/python -m tools.matchsearch propose build/matchsearch/player \
    build/player-candidate.c --parent root --label field-view \
    --hypothesis "Test a typed history field instead of repeated byte-offset arithmetic"
build/venv/bin/python -m tools.matchsearch diff build/matchsearch/player field-view --limit 100
build/venv/bin/python -m tools.matchsearch export build/matchsearch/player field-view build/review.c
```

`propose` prints a compact verdict and changes. `diff` shows the ordinary
production instruction stream against the ROM, with changed rows and nearby
context. It lazily obtains scope's C-line, RTL UID and first-seen-pass annotations.
Those annotations are attached only when the debug compile has exactly the same
bytes and decoded rows as production; otherwise the listing explains that they
were withheld. `--no-annotations` skips that extra compile. An instruction's
first-seen pass is provenance, not a claim that the pass caused its mismatch.
Listing alignment remains a heuristic. Use `--json` on these commands for their
structured output, or `propose --local` to skip the resident socket.

Export writes the stored source verbatim. A hand-authored proposal retains its
comments and macros. A catalog move regenerates its edited AST definition before
storing the node, so that definition may already have expanded macros and lost
comments; export cannot reconstruct them. Structs, macros, helper extractions and
semantic repairs can always be submitted as ordinary source files.

For an exact candidate in a session initialized from an existing `src/` unit:

```bash
build/venv/bin/python -m tools.matchsearch land build/matchsearch/player field-view --dry-run
build/venv/bin/python -m tools.matchsearch land build/matchsearch/player field-view
```

The preview saves a concrete merged source and stock verification evidence under
the session's `landing/` directory. Landing uses the stored baseline for a
three-way merge with the current file, unwraps the selected NONMATCHING function,
and checks that its C definition is active in the normal build. It requires a
fresh ordinary match and independent stock verification of the merged source.
Then it formats only that unit and runs `tools/check.py` on it. A failed tool,
timeout, exception, or Python interruption restores the original bytes. The
original, proposed and landed sources and command logs remain in the evidence
directory. Landing does not commit, push, or run the combined ROM build.

Concessions are counted after preprocessing, including referenced inline helpers.
Pins hidden behind macros therefore still add the unit's concession marker.
An existing marker is retained because another function may still need it.
Automatic landing of instruction asm is unsupported; SDK/original-asm cases need
their existing source-review workflow. New units and renames remain scope's
responsibility. Avoid editing the same unit concurrently with a landing command.

## Resident protocol

The command-line entry point is `python -m tools.matchsearch`. Its commands are
`init DIRECTORY SOURCE FUNCTION`, `request DIRECTORY JSON`, `status DIRECTORY`,
`serve DIRECTORY`, `stop DIRECTORY`, `history DIRECTORY OUTPUT`,
`fork PREVIOUS DIRECTORY`, and `import HISTORY DIRECTORY`, plus the worker
commands above. `request --file FILE` avoids shell
escaping. The socket service keeps the compiler warm across requests. If the
execution sandbox disallows sockets, the client falls back to local execution;
the graph is still persistent. `serve --stdio` keeps a resident compiler without
using sockets, one JSON request and response per line.
Restart resident workers after upgrading Python tool code; an existing session
keeps the implementation and analysis version loaded when it started.

For example, a Python caller can keep the stdio service alive:

```python
import json
import subprocess
import sys

worker = subprocess.Popen(
    [sys.executable, "-m", "tools.matchsearch", "serve",
     "build/matchsearch/player", "--stdio"],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, bufsize=1,
)

def request(op, **args):
    worker.stdin.write(json.dumps({"op": op, "args": args}) + "\n")
    worker.stdin.flush()
    return json.loads(worker.stdout.readline())

print(request("status"))
print(request("moves", node="root", families=["compare_reverse"], limit=8))
print(request("node", node="root", analysis=True))
print(request("shutdown"))
worker.wait()
```

| Operation | What it does |
|---|---|
| `status`, `frontier` | Counts and contrasting candidates; prefixes or unique labels identify nodes. |
| `node`, `source`, `target`, `context` | Inspect stored source, target constraints, and frozen environment. `node(analysis=True)` returns the full regional comparison. |
| `contracts` | Inspect frozen baseline call prototypes, their provenance, and explicit ABI assumptions. |
| `moves`, `expand` | Enumerate deterministic AST edits and compile a selected slate. Filter by families and AST subtree or source-line region. |
| `propose` | Add a free-form model edit, including semantic repairs, helper changes, and struct reconstruction. |
| `compare` | Source diff, numeric changes, region changes, and both stored node summaries. |
| `boundaries` | Call-anchored instruction regions, incoming/outgoing value obligations, physical bindings, and explicit stack-transport assumptions. |
| `diagnose` | Lazy compiler priorities/reloads/PRE and compiler-source references; `knobs` creates separate counterfactual evidence. |
| `decision_delta` | Named allocation/lifetime changes and reload-set changes, without pairing unstable pseudo numbers. |
| `semantic` | Sampled execution against the ROM, including coverage and counterexample details. |
| `retain`, `reject`, `protect` | Explicit policy decisions. Protections report changes or unknown correspondence; they do not silently discard variants. |
| `evidence` | Compact evidence index by default. Supply `evidence_id`, or `full=True`, for complete stored evidence. |
| `verify`, `export` | Recompile with stock agbcc; export reviewable C. Neither command edits the source unit automatically. |
| `refresh` | Recompute stale analysis using cached compiled bytes, or evaluate pending imported nodes. Optional `nodes` and `limit` bound the work. |
| `history` | Return portable source/hypothesis JSON; historical measurements are labelled and cannot become current verdicts. |
| `integrity`, `backup` | Check SQLite structure/graph references, or publish a checked snapshot at a new path without overwriting an existing file. |

`diagnose` defaults to a small summary. Full allocation/conflict data and RTL
dumps remain at the returned paths. Counterfactual graph output is compact by
default; `full=True` exposes it. UID-based knobs use the required debug compile,
and `debug_changes_code` explicitly says whether those annotations describe the
ordinary instruction stream. Diagnostics never overwrite production verdicts.

## Search representation

Each node contains the complete C source under a frozen context. The context
hash includes the ROM, function extent, compiler binaries, assembler and
preprocessor identity, flags, headers, symbol resolution, and relevant existing
tool code. It also records the target's generated assembly presence/content and
resolved ROM data ranges: adding previously absent assembly can change listing
classification without changing a single ROM byte. Reopening under a different context fails instead of serving stale
results. The source graph is in `search.sqlite3`, with rollback journaling and
full synchronization. Opening a store checks database integrity before writing.
Edges retain their
parent, move, hypothesis, prediction, and local safety argument. Multiple parents
are preserved. Workers claim expansions through transactional leases with
generation fencing. Compiler outputs live in separate worker directories.

The large-function run exposed corruption after reopening two independent
WAL-backed graphs in the managed workspace. The underlying cause was not
reproduced in a standalone SQLite stress test. The current journal policy keeps
committed state in the main database file; recovered graphs have passed
separate-process reopening and integrity checks. Existing healthy WAL graphs
must be quiescent for migration. Damaged graphs are refused, not repaired or
silently replaced. Use the SQLite backup API through the store for snapshots:

```python
from tools.matchsearch.store import Store

with Store("build/matchsearch/large") as store:
    print(store.integrity())
    print(store.backup("build/checkpoints/large-001.sqlite3"))
```

The destination must be new. The backup is checked, synchronized, and published
atomically; it is independent of sidecars. A portable `history` export is also
useful because it preserves source hypotheses independently of binary caches.

The evaluator freezes all project include files plus the baseline's external
header dependencies. It checks actual CPP lookup paths, including absent paths
where a new file could shadow an existing header. Candidate directives that
introduce dependencies outside that inventory require a new baseline/session.
Dynamic `__has_include`, `include_next`, and framework include search are outside
the supported frozen-input contract and are rejected explicitly.

The graph can contain cycles: an inverse edit can return to an earlier source.
Analysis has a separate version from physical compiler inputs. Analyzer upgrades
mark old metrics stale and remove them from frontier ranking until `refresh`;
the byte cache usually avoids another compile. A compiler, ROM, header, symbol,
or relevant scope-tool change instead requires a new context. `fork` copies the
source graph and hypotheses into that context, then recompiles its root. Other
nodes stay pending until selected or refreshed. Old scores, rejections, region
protections, and worker leases do not become current decisions. Retained source
hypotheses remain marked for attention.

`history` works directly from SQLite even if the old compiler context is no
longer available. Its JSON includes source text, edges, and historical summaries;
it excludes machine-code bytes, disassembly analysis, and compiler trace files.
Use `import` in another checkout, with `--source` when the original source was
outside the repository. The original history is never rewritten by a fork.

**Equal machine code does not mean an equal search state.** Two C forms can
compile to identical bytes and offer different next edits: a scope, helper,
declaration, or otherwise eliminated temporary can affect the next compiler
decision. Merging them by binary hash would erase precisely the plateaus this
tool needs to cross. Only their expensive graph comparison is shared; source
nodes and edges remain separate.

The frontier combines explicitly retained hypotheses, objective champions,
structural/region niches, and an exploration slot. Objectives expose dependency,
control, operation, instruction, size, and concession differences separately.
Source token count breaks some ties toward smaller representations; it is not
a measure of historical authenticity. No scalar distance is a proof and no
dominated node is destroyed. The model can expand any stored node directly.

This is deliberately not labelled AlphaGo or a trained MCTS policy. Uncalibrated
rollouts over millions of equivalent C spellings are expensive, and a generic
instruction score is a weak value function. The useful part of the analogy is
preserved: the environment is cheap and exact at the terminal test, the model
chooses hypotheses, and the tool owns memory, branching, transpositions, and
feedback. A future tree policy can use the same graph without changing its
correctness boundaries.

## What the ROM determines

`constraints.py` lifts decoded Thumb instructions into typed definitions and
uses, separate NZCV flags, explicit memory effects, calls, and a CFG. Reaching
definitions propagate through loops and joins; copies preserve value provenance.
Constants, operand roles, branch destinations, signedness, and access widths
survive register normalization. An affine projection recovers simple base/offset
addresses and leaves loads, phi alternatives, and unsupported expressions opaque.
Recognized jump tables preserve case order. Unknown indirect edges stay unknown.

This is a graph of **machine values**, not uniquely recovered original variables.
In the supplied compiler, `reload1.c:42–68` explicitly describes per-instruction
temporary reload registers, spilling, and reassignment. `local-alloc.c`'s
`combine_regs` joins quantities, and its final assignments map their offsets
back to several pseudos. A forward `reg_renumber` table does not invert into a
unique source-variable partition. The tool retains conflicting register
observations instead of resolving them by majority vote.

The compiler's `allocno_compare` in `global.c` uses reference counts, live lengths,
size scaling, and allocation-number tie-breaking. Source edits can change those
statistics before later passes remove their visible effects. Forcing a desired
assignment is a diagnostic intervention, not recovery of the original RTL.
The existing force hook does not itself validate previously assigned conflicts.
These facts argue for inverse constraints and forward experiments together,
rather than an alleged inverse allocator oracle.

Per-region feedback includes live-in/live-out definitions, target/candidate block
alignment, local byte agreement, and dependency compatibility. Region fingerprints
translate producer and successor identities into target coordinates, so unrelated
earlier instruction renumbering does not automatically invalidate a region.
Unmapped producers leave the fingerprint unresolved. Local byte agreement also
does not prove a compatible entry register binding: the complete function still
has to match. Every candidate is compiled as a complete function; isolated block
compilation would change allocation, PRE, reload history, and literal placement.

`contracts.py` infers supported explicit call prototypes from the original
baseline translation unit, then freezes the same contracts for target and
candidate analysis. Changing a candidate prototype cannot erase an inconvenient
dependency. The map records declarations and its 32-bit ARM ABI assumption;
these are not proofs about the ROM callees. Old-style, variadic, conflicting,
attributed, aggregate, and multiword signatures stay conservative. Unknown calls
consume r0–r3 and memory.

### Stack transport and large-function boundaries

`stackflow.py` tracks stack offsets, widths, overlapping stores, SP changes,
push/pop, joins, and loops. It keeps the original physical dependency graph and
adds `transport_dependencies` separately. A proved identity reload can refer to
the value previously stored; a truncating or extending access instead receives
a typed-bit fact unless identity is established. Each alias records its reaching
stores, source values, width, and assumptions. Unresolved aliases and iteration
budgets produce uncertainty, not a guessed identity.

The default `strict` policy invalidates tracked stack contents on unknown calls
and potentially aliasing writes. Optional `abi-private` analysis assumes a
private, non-escaped frame with no forged or dangling pointers or unmodeled
asynchronous stack writes. It preserves only calls whose frozen arity excludes
outgoing stack arguments; other calls still invalidate. The report includes the
full assumption list and both target/candidate policies. It does not change
byte acceptance or justify semantic pruning.

`boundaries.py` aligns external call sequences first, then pairs instructions
inside each interval. This avoids matching one of four nearly identical inline
helper copies against another. Bounded minimum-edit alignment reduces greedy
mis-pairing of repeated store motifs; oversized intervals explicitly fall back
to the existing aligner. Repeated calls and tied alignments remain hypotheses.
Local Thumb `BL` long jumps are CFG edges with a link-register definition, not
external ABI calls. Unreachable predecessors do not inject fictitious entry
values into reachable blocks.

```python
with Session("build/matchsearch/large") as session:
    report = session.boundaries("candidate", limit=8, detail_limit=6)
    conditional = session.boundaries(
        "candidate", region="call:16", policy="abi-private", detail_limit=20
    )
```

Each region reports unmatched instructions and changed scalar operands,
physical dependency differences, and transported live-in/live-out obligations.
Different upstream constants remain different even when the local instructions
are identical. Stack-mediated obligations include the alias provenance needed
to audit the conclusion. Memory effects remain visible. Neither a corresponding
region nor a reduced structural score is an equivalence proof.

## Moves and semantic checking

The default catalog has guarded families for pin removal, declaration order,
scope changes, independent statement order, temporary introduction, copying and
recomputation, commutation, comparison reversal, if/else exchange, boolean
splitting/merging, guard clauses, loop forms, and return/goto forms. The catalog
round-robins families, caps candidates per request, and limits generated locals.
Move IDs are stable for the parent/source/site/result. AST paths identify sites;
line numbers are presentation hints. A move records wider touched sites when it
also inserts a declaration.

The machine-code target and edited C definition can be selected separately.
For a large function assembled from inlined helpers, use
`moves(..., edit_function="HelperName")` and the same argument to `expand`.
The helper must be defined in the supplied source unit. The graph records the
edited definition, but the evaluator still compiles and compares the complete
original target function. A helper in a shared header instead requires an
explicit header edit and a new physical context. This distinction matters in
the 8,764-byte case: much of the machine code comes from repeated inline helpers,
so edits confined to the short outer C body miss most useful source decisions.

Guards are conservative local arguments for defined C, not proofs against the
ROM. They account for lexical bindings, volatile values, truncation, initialization
order, writes to dependencies, control flow, and `continue` when changing loop
forms. The older `rewrites.py` generators remain available only through explicit
`legacy:rw_*` families, labelled hypotheses. Changing a width or moving a memory
access is not silently called semantics-preserving.

`statement_order` also permits a bounded field-store case: adjacent simple
assignments of constant integer expressions to distinct, non-bit-field integer
members of the same locally bound pointer to a uniquely resolved struct.
The struct supplies the disjointness argument. Different pointer bases, unions,
shadowed or unresolved record types, calls, memory-reading right-hand sides,
read/modify/write operations, and address-expression chains remain excluded.
Qualifier checks follow file-scope typedefs, so a hidden `volatile` or atomic
type cannot make an observable access look like an ordinary local value.

**The starting draft need not be correct.** A semantics-preserving closure of an
incorrect draft cannot reach the ROM. The model-proposal path therefore accepts
source repairs as well as equivalent spellings. Every edge states what it tests;
equivalence to the parent is not a universal admission gate.

The existing Unicorn checker supplies sampled evidence against the ROM. Its
memory is synthetic, pointer arguments are normally disjoint, and callees are
stubbed. A counterexample needs validation against the function's actual input
and callee contracts before it justifies pruning. Agreement reports its coverage
and remains a test. Only ordinary byte identity is the definitive matching test.

Each requested semantic check runs in a separate process with a 30-second
wall-clock limit, frozen ROM bytes, target extent, names and address contracts.
Native signals such as SIGILL, timeouts and malformed worker output return
`unavailable` without terminating the search resident. A successful capability
probe alone would not protect against a later native fault, so execution itself
stays inside the child. Runtime failure is never a semantic counterexample.

The execution model freezes baseline declarations, input-parameter sampling,
and return-value observation before checking a candidate. Callee contracts bind
to ROM addresses, then follow the frozen ELF's selected display names, including
`__addr` aliases and indirect-call labels. Candidate declarations cannot change
which arguments the oracle observes. Unknown signatures still observe r0–r3 and
the outgoing-stack heuristic; a four-register contract does not discard a
possible fifth argument. Each result records the model version, contract binding,
engine hash, and observed-callee assumptions. Directed fixtures may explicitly
supplement missing baseline declarations from independently frozen API headers;
that is an additional test-model assumption, not a source or byte-verdict change.

## Modules and validation

| Module | Responsibility |
|---|---|
| `evaluator.py` | Reuses scope fork servers/relocation, freezes dependencies, distinguishes diagnostic and production verdicts, verifies with stock agbcc. |
| `constraints.py` | Machine-value graph, CFG, operand/dependency-aware comparison, affine address facts, region contracts. |
| `stackflow.py` | Conservative stack-value transport with separate strict and explicitly conditional private-frame policies. |
| `boundaries.py` | External-call alignment, bounded instruction correspondence, and incoming/outgoing value obligations. |
| `contracts.py` | Conservative, baseline-derived call-register contracts with declaration provenance. |
| `moves.py` | Guarded finite AST slates over the existing parser, deterministic export, explicit legacy hypotheses. |
| `store.py` | Durable directed source graph, context binding, provenance, portable histories, claims and generation fencing. |
| `frontier.py` | Diversity-oriented view over all retained source nodes. |
| `session.py`, `__main__.py` | Policy API, slate execution, compact evidence, JSONL/socket service. |
| `worker.py` | Readable production diffs, exact-source export conveniences, and local merge/format/unit-check landing with rollback. |

The synthetic tests protect actual failure boundaries: changed dependencies
hidden by register blanking, carry flags, changed branch/switch targets,
assignment truncation and shadowing, diagnostic matches entering the real
frontier, stale contexts, concurrent claims, expired workers, and restart
recovery. They contain no ROM bytes and are not claimed as decompilation progress.

Run all matching-search tests with the standard library runner; pytest is not
required. Normal project setup supplies the runtime dependencies.

```bash
build/venv/bin/python -m unittest discover -s tests -p 'test_matchsearch_*.py' -q
```

The executable semantic cases run through the isolated worker and skip explicitly
when its native backend is unavailable. Signal/timeout containment tests still
run. CPP regressions use a compiler driver and explicit input, including on macOS.

Real-function results and measured costs are in
[`notes/matching-search_2026-10-29.md`](../../notes/matching-search_2026-10-29.md).
The largest-unmatched-function experiment and its unresolved compiler decisions
are in [`notes/matching-search-scale_2026-10-29.md`](../../notes/matching-search-scale_2026-10-29.md).
Compiler throughput and full dependency-graph latency are reported separately;
the 12-ms shorthand for small fork compiles is not a promise for every stage of
a large-function evaluation.
