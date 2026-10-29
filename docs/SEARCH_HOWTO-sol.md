# Stateful matching search (Sol)

Tags: #tooling #matching #compiler

Run from the repository root. The model chooses the policy; the tool retains
competing source branches, including equal, worse and failed candidates.

```sh
search() { build/venv/bin/python tools/scope/search.py "$@"; }
search --session actor init src/UNIT.c FUNCTION
search --session actor values --limit 6       # addresses, types, calls, comparisons
search --session actor regions                # local dependencies versus global bindings
search --session actor listing --limit 20
search --session actor why                    # actual compiler diagnosis
search --session actor moves --limit 20       # IDs, source sites, legality conditions
search --session actor expand MOVE_ID --parent ROOT --reason 'test this hypothesis'
# Edit the whole unit, preserving helpers, includes and NONMATCHING guards.
search --session actor try build/a.c build/b.c --parent NODE --reason 'compare one idea'
search --session actor trace --a NODE         # selected-node compiler decision changes
search --session actor priority --argument table --competitor '=24'
search --session actor demand                 # reload pressure and final evictions
search --session actor land EXACT_NODE        # merge and production unit verification
```

`expand` and `try` retain siblings without selecting a winner. `import` selects
its child; `checkout` selects any retained node. Use explicit node IDs when sharing
a session; hash prefixes work. `moves --kinds commute introduce-temporary` filters
families; `expand --kinds commute --limit 4` compiles a chosen neighborhood warm.
Moves preserve defined behavior under their stated conditions. Unknown types,
volatile accesses, calls and memory are not assumed pure. Free edits remain unproved hypotheses.

Solve calls/control paths, memory widths, signedness, addresses and values first.
`values` exposes uncertainty; call arity remains unknown. `regions` separates local
dependencies from global bindings. `protect B0 B2` records solved shape obligations;
`--mode binding` includes registers. Violating branches remain available.
`focus --lines 40:70` restricts move sites; `focus --lines clear` clears it.
`twins --region B4` retrieves matched C prior art without proving equivalence.

`trace` pays for diagnostic RTL on selected nodes and points at agbcc source.
`priority` uses actual local quantities, whose birth/death intervals differ from
flow's pseudo lifetimes. Rank thresholds hold other metrics fixed; suggested
registers, classes and conflicts still matter. `demand` distinguishes actual
reservations/evictions from inferred pressure causes. `--details` retains records and ambiguity; `--json` returns response envelopes. `compiler --view life
--argument value`, and views `frame`, `hoist`, `giv`, `pre`, `probe`, `spill`, reuse
scope. Raw dumps are isolated by source, context and query; `trace --details`
includes their `artifacts` directory. Earliest changed pass is not causal proof.

For a precise CSE RHS query, `experiment --knob CSE_SUBST_TRACE=1` records changed
candidate attempts; `CSE_NOSUBST=1:104,1:105` suppresses selected pass:event pairs.
Identity candidates remain valid; alternatives may require separate suppression.
`AGBCC_NOLOCAL=143` leaves the quantity containing pseudo 143 for global allocation.
These are compiler counterfactuals, never matches.

`experiment --knob AGBCC_PRI=value:+100` stores a separate counterfactual. Only
ordinary compiler bytes equal to the ROM mean exact. PRI/FORCE and CSE pseudo
selectors accept unique names; UID/event selectors require numbers. Unsupported
formats are rejected. `evaluate --trials 16` runs differential Thumb execution in
a bounded subprocess without recompiling. EQUIVALENT is sampled evidence;
INCOMPLETE, TIMEOUT and UNAVAILABLE cannot certify equivalence. The current sandbox cannot run Unicorn; unavailable execution is never proof.

Keep intent with `note '...' --kind hypothesis`; inspect `frontier`, `history` and
`notes --all`, or `export --out build/search.json`. `checkout NODE --out FILE`
selects a retained source. Sessions live in `build/search-sol/NAME/`; never commit
runtime ROM-derived data.
The mailbox resident starts automatically and restarts after implementation
changes. Each session has its own resident, so one long query does not block others. `stop` stops it; `batch` reads JSON commands line by line;
`SCOPE_SEARCH_SERVER=0` runs locally. Warm compiles take milliseconds; tracing and startup add overhead.

`land` forces an ordinary evaluation, prepares campaign inputs from the immutable
root, formats/merges the source, and requires production unit MATCH before writing
`done`. A blocker is an open function; keep working until MATCH. Failure
restores the source. Initialize from the actual `src/` draft so the merge base is
correct. If a scratch root already contains edits and conflicts, initialize a new
session from the production unit and import the exact candidate. Clean and
inspect the exact candidate first. Concessions are counted after production
preprocessing, including macro-hidden pins in referenced helpers. Landing does
not commit or run a full ROM build. Changed compiler inputs or target assembly
classification create new contexts, never stale cache hits.

[Design, evidence and remaining gaps](../notes/search-sol_2026-10-29.md).
