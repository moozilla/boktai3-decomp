# Stateful matching search (Sol)

Tags: #tooling #matching #compiler

Run from the repository root with `build/venv/bin/python tools/scope/search.py`.
Below, `search` means that command. The model chooses the search policy: the tool
retains all branches, including equal, worse and failed candidates. Byte equality
with the ROM is the only exact verdict. Start with an actual NONMATCHING draft.

```sh
search --session actor init src/UNIT.c FUNCTION
search --session actor regions              # dependency shape, bindings, concrete differences
search --session actor listing              # concise ROM/current machine differences
search --session actor why                  # actual compiler diagnosis, saved in session notes
search --session actor moves --limit 20     # stable IDs, sites, legality conditions
search --session actor expand MOVE_ID --parent ROOT --reason 'test this compiler hypothesis'
search --session actor frontier             # no greedy acceptance or automatic score pruning
search --session actor checkout NODE --out build/actor.c
# Edit build/actor.c; include helpers/types and retain the unit's guards.
search --session actor import build/actor.c --parent NODE --reason 'explain the hypothesis'
search --session actor trace --a NODE       # actual allocation/reload/pass changes against NODE
search --session actor diff NODE HEAD
```

`expand` keeps HEAD where it was; select a child with `checkout`. `import` selects
its new child. Use explicit node IDs when sharing a session with another worker.
Node hash prefixes work. `moves --kinds commute introduce-temporary` filters
families; `expand --kinds commute --limit 4` evaluates that chosen neighborhood
in one warm batch. Moves preserve defined C behavior under their listed conditions;
unknown types, calls, memory and volatile accesses are not assumed pure. The first
AST move normalizes the selected function's formatting and removes its comments.
Free edits remain explicitly unproved; inspect them and use semantic trials.

Solve the machine constraints first: call targets/control paths, memory widths,
signed loads, offsets and values. These are observations, not unique source types
or variables. `regions` compares basic blocks with dependency-sensitive value IDs,
so changing earlier registers does not erase local shape progress. `regions
--details` includes CFGs, reaching definitions and ambiguity. A local shape match
is neither behavioral proof nor exact bytes. `protect B0 B2` records solved local
shape obligations; `--mode binding` also protects hard-register bindings. Future
evaluations report violations while retaining the branch. `protect --clear` clears
them. `focus --lines 40:70` restricts move sites; `focus --lines clear` removes it.

Use `trace --a PARENT` on selected nodes: it pays for diagnostic RTL once and
reports actual named-value allocation changes, reload changes and the earliest
changed pass with an agbcc source file:line. `--details` exposes all records and
ambiguous correspondences. Earliest divergence is not causal proof. `compiler
--view life --argument 'value'` and views `frame`, `hoist`, `giv`, `pre`, `probe`,
`spill`, or `why --argument UID` reuse scope. `experiment --knob AGBCC_PRI=value:+100`
is a separate counterfactual evaluation; even an experiment match is never exact.

`evaluate --trials 16` uses the existing differential Thumb oracle without another
compile. It reports coverage and counterexamples; EQUIVALENT is sampled evidence,
INCOMPLETE is not a pass, and missing Unicorn reports UNAVAILABLE. Knob experiments
are never semantically certified. Do not optimize the instruction count alone:
combine hypotheses, inspect shape and decision deltas, and return to earlier nodes.
Record intent with `note '...' --kind hypothesis`; `mark NODE --state parked
--reason '...'` manages the frontier. `history`, `notes --all`, and `export --out
build/search.json` preserve the search. Runtime state contains ROM-derived data:
keep it under `build/`, never commit it.

Sessions live in `build/search-sol/NAME/session.sqlite3`; pass a database path to
share one explicitly. The resident starts automatically, uses a sandbox-compatible
filesystem mailbox, and restarts when implementation files change. `stop` stops
it. `SCOPE_SEARCH_SERVER=0` runs locally; `batch` reads JSON commands one per line
and keeps compilers warm without a daemon. `--json` returns response envelopes.
Typical warm compilation is tens of milliseconds; tracing and first-use setup cost
more. Compiler/environment changes create new contexts instead of stale cache hits.
On exact output, export `best_match.c` to the function's campaign directory, retain
`base.c` and queue metadata, then use `campaign.py land FUNCTION .`; it verifies the
unit with the normal build compiler. No full ROM build is needed in this worktree.
