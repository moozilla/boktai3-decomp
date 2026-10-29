# Stateful matching search engine (Sol)

Tags: #matching #tooling #compiler #process

Worker entry point: [SEARCH_HOWTO-sol.md](../docs/SEARCH_HOWTO-sol.md).
The implementation is `tools/scope/search.py` and its `search_*.py` modules.
Runtime evidence and whole-source snapshots live only in `build/search-sol/`.

## Design decisions

The model chooses the policy. Each node is immutable source plus compiler context;
SQLite stores moves, transpositions, evaluations, hypotheses, frontier state and
chronological events. Equal binaries retain distinct source neighborhoods. Worse,
failed and sampled-counterexample branches remain available. Undo can create a
cycle, so this is a general state graph with an append-only history rather than
an artificial DAG. Automatic move enumeration is finite and conservative; unknown
memory, typedefs, calls and volatile accesses are never assumed pure. Free edits
are recorded as unproved hypotheses.

Byte equality with the ROM under the unmodified compiler is the exact verdict.
UID/debug builds and other knob experiments cannot become exact progress or land.
The tool bridges exact nodes through campaign's three-way merge and independent
production unit verification, with source rollback on failure. Whole-unit
production preprocessing counts concessions, including macros and referenced
inline helpers; unsupported extraction cannot certify that code is pin-free.

Hard-register lifetimes do not uniquely identify original source variables:
coalescing, reload and reuse lose that information. Frame layout is likewise an
observation rather than a uniquely determined source constraint. The analyzer
therefore exposes reaching-definition ambiguity, separate local dependency shape
and global bindings, symbolic addresses, signedness, loop recurrences and branch
comparison domains. It does not turn register-erased similarity into a semantic
proof. Twin lookup uses normalized tokens only to retrieve matched C prior art.

Compiler traces retain actual allocator quantities, preferred classes, reload
rotation, evictions and per-pass divergence, with compiler source references.
Earliest divergence and inferred pressure causation are labeled accordingly.
The `priority` query distinguishes flow's pseudo live lengths from local-alloc's
quantity birth/death coordinates and computes rank thresholds under fixed metrics.
Exact reload need vectors, victim decisions and every quantity's tied members
still need additional compiler instrumentation; these gaps are explicit.

The resident uses an atomic filesystem mailbox because this sandbox denies Unix
socket binding. Requests release scope's dump locks, and code changes restart the
resident at request boundaries. Source materializations are content-addressed,
preventing concurrent clients from compiling another client's mutable file.
Compiler/CPP/assembler flags and binaries, headers, symbols, ROM/ELF, loaded
implementations, generated assembly presence/content, and resolved literal-data
ranges participate in context identity. Before/after checks reject changing inputs.

## Evidence and limitations

The first trials used still-unmatched drafts: 0800D874, 0800EF90, 0800FC28,
08022878 and 08110C9C. No solved function was used to claim search effectiveness.
The tilemap trial 08110C9C improved from 13 ROM / 11 draft differing instructions
to 9 / 7 through computation lifetime and inline argument changes. It remains
unmatched; score changes are not exact progress.

After the disk-space interruption, fresh trials took the Monsters tail at
081B7B1C, 08203DF4 and 0820539C, and adjacent Bosses Ratatosk functions 081F3C68
and 081F4CF8. A later separate Vulcan cannon trial took 08207A00. The concurrent seek workers were still at 081875EC and 0817C3F4
when these trials started; none of these targets had a seek session then.

On 08203DF4 the exact local-priority counterfactual exposed a table quantity
with 4 refs and a 98-half-instruction interval, priority 816, competing with
constant 24 at priority 833. Removing two interval ticks or lengthening the rival
by one can cross the tie boundary. The new query directed a source change that
reached one differing instruction, but a partial aggregate copy made that branch
unsuitable as final author-like C. Counterfactual equality is recorded as an
experiment, never a normal match.

On 081F3C68 a retained native branch removed all four original pins and recovered
most local shapes; two allocator/reload experiments brought it to six differing
instructions. It still needs a source-level explanation for a CSE pointer
substitution and a coordinate-clamp copy. On 081B7B1C the baseline is seven
instructions / 22 bytes different. Separate component branches fix table-load
ordering and destination-base ordering, while changing allocator lifetimes and
spill-slot order. Neither is a full solution. All branches remain inspectable.

The emulator backend in the shared environment faults with SIGILL during Unicorn
memory mapping, before executing the function. Differential trials run in a
bounded subprocess and report UNAVAILABLE, retaining successful compiler results
and the resident. There is no equivalence claim from this environment.

Repeated ordinary compilation of 081B7B1C measured 11.275 ms after setup; fresh
variants also pay CPP and assembly costs. `compile_ms` excludes context validation;
`elapsed_ms` includes evaluator overhead. Trace/probe queries deliberately cost
more. The suite contains synthetic correctness tests plus opt-in real unmatched
compiler integration tests; it does not establish matches per hour.

A later 08207A00 trial isolated an additional constant reload that advances the
rotation: keeping a named 48 mask introduces an early high-register copy, while
literal masks remove that request. The literal branch is byte-identical under
`AGBCC_SPILL=53:1`, but remains unmatched in an ordinary build. It is a second
function with an exact counterfactual, not a second exact match.

The final tool checks include real CPP in-place define changes, a fresh evaluator
inheriting shared caches, assembler flag changes, and assembler macro contents.
They invalidate both preprocessing and optimized CPP command caches before new
evidence can be attached. Requested and resolved experiment knobs are recorded;
invalid numeric/event selectors cannot silently become `atoi` zero.

The saved evaluation snapshot contains 595 compile evaluations and 85 diagnostic
traces over eleven still-unmatched functions, plus additional compiler probes.
These are stored evaluation counts, not a complete count of compiler processes.
No ordinary exact match was landed: this batch's code gain is 0.00 percentage
points. Final validation passed 134 search tests and 16 scope tests, including
actual unmatched compiles and configured CPP integration. A real semantic request
returned UNAVAILABLE with native signal 4; the next resident ping succeeded.
An attempted land of the Vulcan counterfactual was rejected without changing src.
Landing cancellation kills the campaign's process group before restoring the
source, preventing a timed-out formatter from overwriting the restored unit.
No full ROM build or compiler patch was performed.

## Next work

Finish ordinary C for the retained near-match nodes and verify through `land`.
Instrument precise local-quantity membership, register-class instruction costs,
and reload need/victim iterations, then expose those decisions through the same
session interface. Prototype-based call contracts must be explicit assumptions;
unknown arity currently remains visible rather than treating stale r0-r3 values
as proven arguments. Restore a working emulator backend and exercise semantic
counterexamples on actual execution before using trials for more than evidence.
