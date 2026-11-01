# scope: an agent-first workbench for matching against agbcc

This file is the spec and the working memory of the scope research thread. It
records what the project owner asked for, what we agreed, what the tool does
today, and what we have learned about agbcc with it. Read it before changing
the tool; update it when a decision or a finding changes.

## Why this exists

The project is about 71% matched (52% without register pinning). Progress has
hit diminishing returns, and most remaining effort goes into trial and error:
edit, compile, compare, repeat. That is what verifiable-reward training makes
agents good at, and it is expensive. Earlier research tools (beam search over
"spellings", a mutation-mined hint ranker, many single-purpose scripts) moved
the same way: more search, more scripts.

The goal of this thread is the opposite: make the compiler *legible*, so that
an agent (or a human) can see why agbcc produced what it did and change the C
in an informed way. The intended unit of work is "ask the compiler a precise
question, get a precise answer", not "try another variant".

Measured by: progress per token and per tool call, judged on live work (new
matches, removed hacks), never by re-solving history.

## What the owner wants (the brief)

The end state of the project:

* **100% natural C.** Byte-identical, but written the way a person would have
  written it for this game: no register pins, no `asm("")` barriers, no
  `*(u16 *)((u8 *)p + 0x172)` field access. Documented hacks are acceptable
  when nothing else works, but they should be few, explained, and ideally not
  tied to one register. Intentional assembly (crt0, SDK asm) is excluded.
* **Readable and organized.** Consistent names, systems grouped into a real
  source tree, shared headers instead of per-file struct copies, enums for
  magic constants, comments that say what code does.
* **Who it serves:** the owner's translation and modding work, a translation
  toolkit for others, later ideas (the other Boktai games and regional
  versions, native recompilation, engine swaps). And the wider GBA decomp
  community, assuming they work with agents heavily.
* **Budget:** as cheap as possible on a fixed subscription. The dream is a
  toolkit good enough that one model thread on a cheap plan can decompile a
  game in a week. Tooling plus skills should let small models do routine
  matching; large models design tools and solve new compiler behavior.
* **Visibility:** the owner wants to follow along and learn how decompilation
  works, so agents should over-explain what they are doing while working.
  Human UX is secondary to agent UX for now.

### The phases (agreed)

Work proceeds **per subsystem**, through all phases while its context is warm,
rather than one phase across the whole ROM:

1. Every function has a draft.
2. Drafts match (concessions such as register pinning allowed as interim).
3. Concessions are removed: natural C.
4. Cleanup: names, shared types and headers, file layout, comments.

Agents may hoist a type into a shared header when it is clearly global;
subsystem boundaries are soft. A possible extra concession tier: "verified
equivalent" (semcheck-proven same behavior, bytes differ), useful for
modding and porting before matching is complete.

The first subsystem is **Bosses** (160 KB of code: 42% natural, 13% pinned,
33% drafts, 12% undrafted at the start).

### Principles for the tool

* **What-if and why, together.** A knob that flips a decision proves *which*
  decision matters (what-if). The tool must also show *what in the C feeds
  that decision* (why): the quantities the compiler compared and the C lines
  they came from. Without the why, agents guess C shapes at a known target,
  which is spray-and-pray by hand.
* **Program structure, not only compiler structure.** Many register
  problems are really missing macros, inline helpers, shared variables or
  wrong types. The agent's job is to find the program the original authors
  wrote; the tool's job is to make the compiler's behavior visible enough
  that the right structure is obvious. `do {} while (0)` blocks are usually
  macros: the fix is to reify the macro, not to ban the shape.
* **Progressive disclosure.** A screenful of summary on stdout; full detail in
  files the agent can grep or jq; handles (insn uids, pass names) to drill
  into. Never thousands of lines of listing by default, never a summary so
  terse it hides what the agent needs.
* **Familiar formats.** objdump-style asm, gcc RTL as gcc prints it,
  gdb-like stepping. Standard shell tools on the artifacts. No private DSL.
* **One call for the common case.** Every tool call costs latency and resends
  cached context. The default command should answer the usual first three
  questions at once.
* **Fast.** Interactive questions in well under a second; parameterized
  searches (when a hypothesis has 20 variants) in milliseconds per variant.
  Search is fine when it tests one hypothesis; it is waste when it replaces one.
* **Knowledge lives in code and next to the code it explains.** Pointers from
  a decision to the agbcc source lines that make it, so an agent reads 30
  lines instead of searching 700k tokens. Any annotation of the compiler
  source must be curated understanding ("how this works"), not appended
  one-off notes.
* **Every match leaves the toolkit better.** When the tool's output surprised
  the agent, or it took several calls to understand something, improve the
  tool so the next agent sees it in one.
* New tool, own directory. Duplicating code from older tools is fine for now;
  old tools are retired in the project's cleanup phase, not here.

### Ideas recorded for later

* A greppable corpus: normalized asm (registers and addresses abstracted) and
  paired C for every matched function, plus draft-to-match history as patches.
  The agent greps it ("which matched function has this 6-insn window, and how
  was its C written?") instead of a tool ranking hints for it.
* Shape vs allocation split: force the ROM's register assignment on the
  draft's pre-allocation RTL. If that reproduces the ROM, only allocation is
  wrong; otherwise the shape (control flow, CSE, loops, types) is wrong. Gives
  large functions a decomposable intermediate milestone. Reload may resist.
* Refactoring as an IDE operation: extract a repeated shape into a header
  inline or macro, rewrite all call sites, recompile every affected unit,
  report matches. Same machinery serves phase 4.
* A watchable session journal (current function, diff, decisions examined)
  rendered for the owner. For now: agents narrate in chat.
* Searchable agent session logs (Claude Code keeps JSONL transcripts).
* Emulator-driven naming of constants (watch a field, see what changes).

## The tool today

```
python3 tools/scope/scope.py FILE.c [FUNC]        real differences, one call
python3 tools/scope/scope.py FILE.c KNOB=V ...    same compile with instrumented knobs (a what-if)
python3 tools/scope/scope.py insn UID [FUNC]      one RTL insn through every pass (FUNC: name, sub_ label or address)
python3 tools/scope/scope.py uses REG... [PASS...] which insns (uid@C line) use a pseudo after each pass
python3 tools/scope/scope.py FILE.c why UID       why insn UID has the registers it has (see below)
python3 tools/scope/scope.py FILE.c why all [RANGE]   why on the first real difference of each hunk (SCOPE_WHY_MAX=24;
                                                  RANGE: 0xLO-0xHI ROM addresses, +0x40-+0x80 offsets, 120-180 C lines)
python3 tools/scope/scope.py FILE.c spill         is the whole difference reload's spill rotation?
python3 tools/scope/scope.py FILE.c life NAME...  the insns flow counts toward a pseudo's live length, by C line
                                                  (a pseudo cse/gcse removed: its sets and uses before that pass)
python3 tools/scope/scope.py FILE.c pre           gcse's PRE replacements by expression: C names, pool values, where
                                                  it is computed and inserted, the lines replaced, AGBCC_NOPRE=k scores
python3 tools/scope/scope.py A.c vs B.c [FUNC]    two drafts' allocation side by side: rank, priority, refs/live, prefs, register
python3 tools/scope/scope.py FILE.c pins [FUNC]   FUNC's pins / barriers (and its inline helpers'): each alone, grouped, all; greedy removal
python3 tools/scope/scope.py FILE.c try V.c...     score whole-file variants (or `try KNOB={a,b}`: one variant per knob value), best first (by shape distance), ~12 ms each
python3 tools/scope/scope.py FILE.c insn UID      (also `FILE.c uses REG...`): compile FILE first, then show
SCOPE_TRANSFORM=tools/scope/transforms/X.py scope.py FILE.c   what-if over the preprocessed source
python3 tools/scope/scope.py FILE.c shape [FUNC]  the diff with low registers blanked: order and shape differences only
python3 tools/scope/scope.py FILE.c frame [FUNC]  stack slots, ROM against draft, with the pseudo (C variable) reload put in each
python3 tools/scope/scope.py FILE.c hoist [FUNC]  loop-invariant motion pass by pass: what loop.c moved out, what stayed, and why;
                                                  then for each hoisted variable it scores the dead re-set before the loop's closing brace
python3 tools/scope/scope.py todo [N] [addr|prior|size]   functions with no C yet, in ROM order (default) or by prior art/size
python3 tools/scope/scope.py new FUNC             open FUNC: build/scope/work/FUNC.c (its unit with a stub, or a like twin's C)
python3 tools/scope/scope.py land FUNC [--name N] [--rom] [--nonmatching] [--allow-pins]   put the draft into src/ and check it
python3 tools/scope/scope.py rename FUNC NEW      functions.csv plus every C spelling of the old name or sub_ label
python3 tools/scope/scope.py like FUNC|ADDR [N]   matched functions sharing the most rare 3-insn windows (helpers to reuse)
python3 tools/scope/scope.py rom FUNC|ADDR        ROM listing, literal pools as data, calls named (no draft needed)
python3 tools/scope/scope.py fields OFF... [STRUCT]   structs with members at these offsets (agbcc-measured layouts)
python3 tools/scope/scope.py census [CALLEE]      register-pinning concessions and drafts grouped by the call they serve
```

`life` takes C variable names or pseudo numbers (names survive edits, numbers
shift). It walks flow's block liveness backward with flow.c's own rule (one
per insn where the register is live before it, plus one where it is set), so
its count equals the `Register N used R times across L insns` line; compare
two drafts by name to see which C line gained or lost an insn.

`pins` (phase 3) looks only at the concessions that can change FUNC's code: those
in its definition and in the unit's inline helpers it reaches by calls (it says how
many others the unit has; earlier it offered those as free removals). It compiles
the file once per concession with that one removed, then once per group whose
members may only work together: every concession of FUNC and its helpers, each
helper's, and a pin with the barriers on the same variable. It writes
`build/scope/drafts/FUNC.no_pins.c` (all of them removed: the near-miss draft a
rematch starts from), then removes greedily (singles and groups, cheapest first,
keeping each removal that adds nothing) into `FUNC.fewer_pins.c`. It names helpers
other functions of the unit also use, and header helpers with concessions FUNC
calls (a header edit: check every unit including it). Variants stay in
`build/scope/drafts/.pins/` for `why`. It handles one-line pins
(`register T x asm("rN")`, the `register` keyword is kept) and one-line
`asm("" ...)` statements; suggested by the second Haiku trial. It supersedes
`tools/depin.py` and the earlier depinning worker's group sweeps: the free
removals are used up, and what is left is rematching from `FUNC.no_pins.c`.

`SCOPE_TRANSFORM` points at a module with `transform(text) -> text`, applied
to the preprocessed source before compiling: a what-if that leaves the file
alone. `transforms/random_table_literal.py` writes `gUnk_0203B400` as the
integer address (see the literal survey below).

`why UID` recompiles with `CSE_WHY=UID CSE_DEBUG=1 CSE_PATHS=1` and
`AGBCC_DUMPPRI`, and prints in ~10 lines:

* the ROM and draft instruction, and which pseudo holds the differing register
  (operands matched by position between the lreg dump and the same insn after
  reload, so local-alloc pseudos are covered);
* the insn's registers after every pass that changed them, pseudos named by C
  variable (`24 (phase)`);
* for a cse change: the equivalence class at the first look (cheapest first,
  each member with its class head), each look's extended block and branch
  path with C lines, and the class-head takeover that decided it, phrased as
  "23 (result) became canonical over 24 (phase) at insn 622 (line 67): its last
  use, insn 583 (line 104), is later than ..." plus the three levers;
* for an allocation difference: the pseudo's global/local record (rank,
  priority, refs, live length, calls crossed), with flow's own live length
  when allocation sees a different one (doubled for a constant register,
  fact 10; changed by regmove), then the first of:
  * reload evicted it from global alloc's choice and re-placed it (fact 13),
    plus which value the ROM keeps in the draft's register and whether the
    two are ever live together in the draft;
  * who already held the ROM's register during its life (hard conflicts
    come from local alloc and fixed uses);
  * the rival: the conflicting pseudo the ROM keeps in the draft's register,
    which global alloc ran after this one (and whether the draft's register
    was a preference, fact 11), or an earlier-allocated pseudo that took the
    ROM's register;
  * where a preference came from: the insn where expand_preferences merged
    it in, with that insn's REG_DEAD notes in order (fact 11);
  * find_reg's own view (fact 16, from the compiler's F record): conflicts,
    the registers pass 0 may reuse, the ones a later pseudo prefers, and which
    test kept the ROM's register out. When the ROM's register was simply not
    in use yet, it names the pseudo that takes it later (preferring one the
    ROM also keeps there) and the arithmetic to allocate that one first;
  * in both rival cases the arithmetic to flip the order: refs or live length
    each side would need, live lengths in flow's units, and the
    `AGBCC_PRI`/`AGBCC_FORCE` what-if that confirms it;
* for a reload register (operand not a pseudo before allocation): the reload
  choice number, the spill registers as that choice saw them (free, shared
  with another reload of the insn, holding a named pseudo, taken), the
  previous three choices, and then either the concrete `AGBCC_SPILL=K:D` that
  gives the ROM's register and the register the ROM's previous choice must
  have taken (`the ROM's r3 means its previous choice took r2, not r0`), or
  why no rotation gives it: a pseudo lives there (named), another reload of
  the insn has it, pass 0 shares a register first, or it is not in the
  function's spill set at all (SceneActorControl_UpdateWanderTarget's r3).
  Inherited reloads since the previous choice are listed (they make no
  choice); `why` on an inherited reload names the insn it reuses.

`spill` greedily shifts reload's spill rotation (`AGBCC_SPILL=K:D`) before each
reload choice from the first differing insn on, keeping shifts that shrink the
diff. Each choice tries only the shifts that give it a different register (one
per free register, from its X record), and the choices next to differing insns
go first, so the budget is not spent on early choices (Player_GetSmoothedDirection:
1 compile, was 7; GridAttackActor_OnDamageCollision: 2). A MATCH means the C
has the right shape and pseudos and only reload's rotation state differs.
Before searching, `spill` names any low register the ROM reloads into that is
not in the draft's spill list. `finish_spills` builds that list from every
register some insn's `find_reg` picked, so a list one register shorter shifts
nearly every later choice, and no `AGBCC_SPILL` shift can fix it. The line names
the pseudos that hold the register in the draft. In OrbitEffect_UpdateManager,
the ROM rotates over r0 to r3, but in the draft a dead field-store zero held r3
and the list was r0 to r2 (130 / 126 differing). On a matching function,
`spill` prints the spill list and stops.

The rotation records come from `AGBCC_DUMPPRI`: after each `R` line (choice K,
insn, register, C line, value) an `X` line gives the index the scan started
after and each spill register's code (`S` shared, `A` free, `L<n>` pseudo N
lives there, `H` a hard register lives there, `T` another reload has it, `C`
class, `M` mode); `I` lines record inherited reloads (insn, pseudo, spill
index, the insn whose reload it reuses).

It re-executes itself under `build/venv` for capstone. FUNC defaults to the
function defined in the file's `#ifdef NONMATCHING` block; the ROM side is
found by name, or by the `INCLUDE_ASM` name in the `#else` branch.

One compile with the instrumented agbcc (`-g -dp -da`) yields:

* the code, linked at the ROM address (literals and calls resolved);
* the C line of each instruction (DWARF line table of the object);
* the RTL insn uid of each instruction (`-dp` comments in `u.s`);
* the RTL after every pass (`u.i.rtl` ... `u.i.mach`).

Both sides are disassembled with capstone. Literal loads compare by value
(`ldr r0, =0x16F`), branches and jump-table words by where they land after
alignment, so offsets that moved only because code shifted are hidden and
counted. Each row in a hunk shows ROM, draft, C line, insn uid and the pass
that first produced that insn (`rtl` = expand, `gcse`, `cse2`, `combine`,
`greg/reload`, `mach` = late splits and reorg).

Artifacts in `build/scope/FUNC/`: `diff.txt` (whole aligned listing),
`diff.json` (same rows), `u.s`, `u.i.PASS`, `stderr.txt` (knob traces such as
`CSE_DEBUG` land here). ROM symbols and the function inventory are cached in
`build/scope/cache/`.

Knobs of scope's instrumented compiler (`tools/scope/build_agbcc.sh`): `AGBCC_NOGIV=U1,U2` /
`AGBCC_GIV=U1,U2` (loop.c: keep the giv set by insn U unreduced / reduce it anyway;
`AGBCC_LOOP_TRACE=1` lists each giv with `fn=`, type, benefit and decision; added after
sub_0803D430's worker asked for a strength-reduction explainer, where NOGIV=66 alone takes
26 to 23 ROM insns; no-knob output identical on 400 loop units and 458 NONMATCHING files; caveat from that worker's second round: the NOGIV=66 hit was the player-slot
address, which the ROM *does* step, so the gain came from allocation side effects; a probe hit on
a giv says the decision matters, not which side the ROM is on: check the ROM listing for a
post-increment or a stepped register before writing C; it reached 24/25 by C, not a match),
`AGBCC_NOMOVE=U1,U2` (loop.c: leave the invariant set by insn U in the loop; with
`AGBCC_LOOP_TRACE=1` the trace also prints `loopopt fn=` once per loop pass, a `cand` line
for every pseudo set in each loop (may_not_optimize, maybe_never, used_before, in_block,
set_in_loop, invariant) and a `move` line with the threshold arithmetic; `probe` flips each
hoist and `hoist` prints the table), reload1.c `S` lines in names.txt (each stack slot:
pseudo, sp offset, size, allocation order; `frame` reads them),
`CSE_NOCANON=N`
(new reg N never becomes a class head), `CSE_KEEP=N,...` (keep N canonical),
`CSE_DEBUG=N` (trace `make_regs_eqv` for new >= N), `CSE_PATH=1` (trace each
jump cse follows), `CSE_NOFOLLOW=K` (refuse the K-th follow),
`AGBCC_GCSE_T=N` (gcse hash table size), `AGBCC_PRI`, `AGBCC_FORCE`,
`AGBCC_DUMPPRI`, `AGBCC_SPILL` (local-alloc / reload; see the sources for
syntax). Added in this thread (in `tools/scope/agbcc.patch`):

* `CSE_WHY=UID`: for each SET of insn UID, every time cse processes it, print
  the candidates (`src`, `src_folded`, `src_eqv`, `src_related`) with costs
  and the equivalence class in order, each member tagged with the insn at
  which it entered the table. This is the "why" for a cse substitution.
* `CSE_NOFOLLOW_JUMP=UID[:PASS],...`: refuse every follow of those jumps,
  optionally only in cse pass 1 or 2. Only the follow across an unconditional
  jump (the BARRIER case) is refused; conditional branches on a path and
  skips around a block are path enumeration, which this knob does not touch.
* `CSE_PATHS=1`: print every extended block cse processes, as
  `path pass=P from=UID to=UID cuid=A..B branches=N: 21A 102A 620N 623T`
  (T taken, N not taken, A skipped around).
* Trace markers: `cse pass 1 FUNC` / `cse pass 2 FUNC` separate the passes in
  stderr whenever a cse knob is set; `eqv` lines (CSE_DEBUG) carry `at=` (insn
  being processed), `newlastuid=`/`firstlastuid=` (the last-use insns
  compared) and `head=` (the class head after the decision); `CSE_WHY` class
  rows carry `canon N`, the member's current class head.
* `AGBCC_NOREG=REG,...`: local and global allocation leave those pseudos
  without a hard register. Reload then gives each one a stack slot, or
  substitutes its REG_EQUIV value (a constant or a memory read) at every use.
  A ROM operand that is read from memory straight into a reload register, with
  no allocated register of its own, looks like this (`mov r3, r8; ldrh r3,
  [r3]; adds r0, r1, r3` in ActorMarker_LaunchFragments).
* `AGBCC_COMBKEEP=REG,...`: combine never merges an insn that sets one of
  those pseudos into another insn (the what-if for a load or copy the ROM
  keeps).
* `AGBCC_RTRACE=1`: a reload trace in stderr: pseudos kicked out of spill
  registers (`RT kick`), each spill register chosen for an insn (`RT insn U
  spill rN`), the spill cost order at each insn with reloads (`RT order`,
  ascending uses), and the output-reload deletion decisions (`try
  delete_output_reload`, `inherit rN store=UID`, `dor ...`).

With no knobs set the instrumented compiler emits the same code as agbcc
(checked on 62 files, default and NONMATCHING, after these additions; again
on 80 compiles after the trace markers and `CSE_PATHS`).

**scope has its own compiler.** `tools/scope/agbcc.patch`, built by
`sh tools/scope/build_agbcc.sh` (20 s) into `build/scope/agbcc/gcc/agbcc`. It
began as a copy of the shared `tools/patches/agbcc-instr.patch` (rasolve,
agbserve), which other threads keep extending. When both lived in one file,
merging main into this branch meant re-merging and re-verifying a compiler
(2026-10-24), so they were split: changes on either side never touch the
other. Ideas from the shared patch (e.g. `AGBCC_WHYREG`, `AGBCC_PREF`) can be
ported here deliberately. Incremental rebuild after editing one file:
`make -s normal BASE_CFLAGS="-O2 -std=gnu11 -Werror-implicit-function-declaration
-Wno-error=incompatible-pointer-types -Wno-everything"` in
`build/scope/agbcc/gcc` (2.5 s); then regenerate that file's section of
`tools/scope/agbcc.patch` (`diff -u --label a/F --label b/F`).

### The workspace: new, land, rename

`new FUNC` finds where FUNC's C will live: an `INCLUDE_ASM` line in a unit,
or nowhere yet (a new
`src/fn/sub_ADDR.c`). It writes `build/scope/work/FUNC.c`: the whole unit with
FUNC's `INCLUDE_ASM` replaced by a stub (signature from the first
declaration in the unit or its headers), plus `FUNC.base.c`, the unit as it
was. Outside a unit, the draft is a like twin's file (coverage >= 0.5) with
the twin renamed, or a stub. Every scope command run on the workspace file
defaults to FUNC, not the unit's last function. The unit suffix is cut at
FUNC's closing brace, so a variant in a 30-function unit compiles in ~38 ms.

`land FUNC` requires a match, and refuses one that adds register pins or
asm barriers to the compiled code (drafts don't count) unless given
`--allow-pins`; the refusal points at `pins` and docs/WORKER.md. It three-way merges the draft into the unit
(`git merge-file` against `FUNC.base.c`, so edits made to the unit since
`new` survive) or writes the new file and the hidden function's replacement
end. Then it adds or removes the concession marker, runs clang-format, and
checks every C function in the landed file (restoring everything on a
failure). With `--name` it renames, and with `--rom` it builds the ROM.
`--nonmatching` lands an unfinished draft for phase 1: the definition goes
under `#ifdef NONMATCHING` with its `// fuzzy:` line (check.py's figure,
which progress.py reads) and scope's shape distance, and `INCLUDE_ASM` stays
in the `#else`. A hidden function cannot land this way (it has no
`INCLUDE_ASM` to fall back on).

`rename FUNC NEW` writes or updates the functions.csv row and rewrites every
whole-word spelling of the old name, of `sub_ADDR` and of any `#define X
sub_ADDR` alias (the define is dropped) across `src/` and `include/`. It
refuses a name that is already in use. A hidden function can take a name:
disasm.py and split.py read it from the csv.

ARM functions (sub_08000430, `// COMPILER: agbcc_arm`) list with ARM decoding
in `rom`, and `todo` marks them `[ARM: no scope compile]`; scope's compiler
is Thumb only.

### Speed (measured on an M-series Mac)

The target agreed for the tool: interactive questions well under a second,
and milliseconds per variant when a hypothesis has many variants.

| Stage | Time |
|---|---|
| agbcc, plain / with all 14 RTL dumps | 7 ms / 19 ms |
| agbcc fork-server child (declarations parsed once) | ~3 ms |
| preprocess (the driver's own cpp command, run directly; memoised per file mtime) | 9 ms (`cc -E`: 23 ms) |
| a draft's C after its `#include` block (fast path: cached include block, body passed through) | <1 ms |
| one search variant, a new file (fork server, `as`, Python placement, align) | ~12 ms (was 51 ms; ~120 ms before that) |
| one search variant, same file and a knob | ~10 ms |
| a diff with all dumps, through the resident server | 0.12 s (was 0.30 s; 0.7 s before that) |
| `probe` (25 variants) / `spill` (42 variants) | 0.36 s / 0.60 s (were 2.6 s / 4.9 s) |

How (2026-10-24):

* **Resident server** (`scope.py serve`, started automatically by the first
  call; `SCOPE_SERVER=0` opts out). It keeps the ROM, ELF symbols, inventory
  and capstone loaded, and forks one child per command, so commands from
  several workers run side by side. The client is the top of scope.py:
  stdlib only, it forwards argv, cwd and the `SCOPE_`/`AGBCC_`/`CSE_`
  environment and prints what comes back. The server restarts itself when
  scope.py or fast.py changes and exits after 30 idle minutes
  (`SCOPE_SERVE_IDLE`). Log: `build/scope/serve.log`.
* **Fork server** (`fast.py`, scope's own copy of the idea in
  tools/agbserve.py): the instrumented agbcc parses everything before FUNC's
  definition once and forks a child per variant with that variant's knobs.
  It gives bytes only (children share the parent's dump files), so `score()`
  uses it for every variant of `probe`, `pins` and `spill`, and the
  commands that read dumps or traces compile once the old way (`full=True`).
* **No ld, objcopy, readelf or objdump**: `fast.place` resolves the
  object's relocations (BL, ABS32, Thumb b/b<cond>) at FUNC's ROM address
  and reads its `$d`/`$t` mapping symbols; `fast.dwarf_lines` decodes the
  DWARF 2 line program. `SCOPE_LINK=1` runs the old linker path.
* **No cpp on a variant**: `fast_preprocess` caches cpp's output for a
  draft's `#include` block (keyed by its text and the headers' mtimes) and
  passes the C after it through with comments blanked, when that C has no
  directive, line splice, trigraph or name of a macro the headers define
  (anything else runs cpp). Checked token by token, with line numbers,
  against cpp on every unit in src/.
* **One server per unit, not per draft**: the fork-server prefix ends where
  the last header ends (the unit's own structs and inline helpers go with
  each request, up to 12 KB), and the main file's name in cpp's line
  markers is canonical. Before this, every variant that edited a helper, or
  was saved under another path, parsed all headers again; past 8 prefixes the
  servers were flushed.
* **Checked**: `scope.py fastcheck N` compiles N units both ways and compares
  each path's bytes with ld's (148/148 identical; the full report was
  identical on 40 drafts). Run it after changing fast.py or the patch.

What remains in a 0.12 s diff: the client's Python startup (~30 ms), cpp
on an edited file, agbcc with dumps, `as`, and reading the dumps for the
listing. A diff through the fork server would need per-request dump files
in the patch.

### Gaps (next work on the tool)

* **Largest-function run (2026-10-29), `080C9C04`**: an isolated private compiler
  experiment suppressing `REG_EQUIV(gRandomTable)` restores the target's `0x150`
  frame, shared `sp+0x14C` table slot, and RNG register roles together. The normal
  source still matches neither that decision nor the whole function. Mutable
  helper inputs, local table forms, shared pointer/union values, and helper
  input/result reuse retain the equivalence. Recovering the source value boundary
  is the concrete remaining gap; the diagnostic patch is not in the normal build.
  A separate audit traces unwanted whole-tail sharing to identical physical
  sequences and `jump2` cross-jumping. `explain_move2add` now treats an extra
  label as one possible cause, alongside different register allocation/history.
  The run also produced `tools/matchsearch` stack transport and explicit
  call-region obligations, with uncertainty retained
  ([evidence and continuation](../../notes/matching-search-scale_2026-10-29.md)).
* **From the first subsystem run (2026-10-25)**: 16 Codex workers' reports, grouped by how many
  hit each (reload/spill causes, PRE source expressions, repeated-block alignment, `why all`'s
  eight-hunk cap, fast-cache races, `-g` knob runs, `land` formatting the whole tree):
  [notes/subsystem-run_2026-10-25.md](../../notes/subsystem-run_2026-10-25.md).
  Done (2026-10-26):
  * Concurrent commands under one `SCOPE_RUN`: the assembler's `f.s`/`f.o`, the include-block
    `head.c` and the pickle caches are per process (writes are atomic renames), and a function's
    work directory is locked for the length of a command, so a second command on the same
    function waits instead of reading the first one's dumps. Fork-server directories are removed
    when their server closes.
  * Knobs compile without `-g` unless they name insn uids (`UID_KNOBS`: NOMOVE, GIV, NOGIV, NOTIE,
    CSE_WHY, CSE_NOFOLLOW_JUMP); 190 knob compiles over 40 drafts gave the same bytes both ways.
    When `-g` changes the function's code (Storage_RefreshConsumableSlots: the `-g` compile
    MATCHES, the build's is 13/13), uid-knob results are marked `[-g compile]` in the listing,
    `try` (mark `-g`, not a MATCH exit) and `probe` (scored against the `-g` count, with a note).
    The main listing's `-g` note says when the `-g` compile itself matches.
  * `pins` keeps a barrier that is the only definition of a variable read later (an output-only
    `"=r"(x)`; PlayerCue_UpdateRecoilSlide's `scratch`) and says so, instead of offering a
    removal that leaves C reading an unset value.
  * The `-g` compile runs with `-Wuninitialized` (same code on 150 units); scope prints agbcc's
    "might be used uninitialized" warnings for FUNC (Vampire_ApplyPendingDamage's `t1`).
    A pinned register variable gets no such warning (it is not a pseudo).
  * `campaign.py land` formats only the landed file (it ran `make format` over the tree).
  * `why all` analyzes up to `SCOPE_WHY_MAX` hunks (24; was a fixed 8) and takes a range of ROM
    addresses, offsets or C lines; the summary names the first hunk left out and the range that
    reaches it. Speed: the dump compile (`compile_draft_g`) now compiles FUNC with the unit cut
    after its body and earlier non-inline functions reduced to prototypes (`diagnostic_source`;
    `SCOPE_TRIM=0` compiles the whole unit). FUNC's code, dumps, names.txt records and lines were
    the same on 199 random units (global label numbers aside). The dump readers (`uid_lines`,
    `insn_list`, `pseudo_names`, `dump_func`, `born_passes`) are memoised on the dump files'
    stamps. Nidhoggr_ApplyPartDamage (4,000-line unit): 10.5 s -> 1.4 s; sub_08193874: over 120 s
    (the workers' "minutes") -> 3.3 s for 24 hunks.
  * Repeated blocks: `align` was difflib's greedy longest block on full instruction text, which in a
    function of repeated blocks anchors on the wrong copy, then paired whatever was left by position
    (`ldr r4, [r7, #0x24]` beside an unrelated `adds r0, r0, r1`). It is now an exact LCS (numpy,
    ~2 ms for 400 insns, 10 ms for 1,000) that pairs the most same-shape insns (low registers
    blanked) and, among those, the most identical ones; leftover insns pair only with the same
    operation and operand form (`sub sp, #0x1c` / `sub sp, #0x20`) and otherwise stay one-sided.
    Same-shape first rather than identical first: identical first gives the lowest count but pairs
    an insn with a coincidentally identical one of the next statement instead of its twin
    (PaletteFade_StartFromScript). Over 149 drafts the differing insns fell 3% against the old
    alignment; some functions count a few more (Storage_RefreshConsumableSlots 13+13 -> 15+15).
    `SCOPE_ALIGN=difflib` gives the old alignment; listings over 6M cells use it. `frame`'s
    slot votes and the order explainer read these rows.
  * PRE in C terms: `pre_records` reads gcse's own dump (expression table, insertions, copies,
    replacements); `pre` prints one block per expression (pseudos named, a literal-pool load named
    by cse's REG_EQUAL note: HealthRestoreEffect_StartParticleBurst's `FlickerParticle_UpdateMotion`),
    where gcse inserted its own computation (end of a block, fact 1), the C lines it replaced and
    each replacement's `AGBCC_NOPRE=k` score. `probe` describes NOPRE candidates the same way, and
    `why` on an insn gcse replaced or created names the PRE decision.
  * `why all` scores each concrete what-if it suggests (`-> 312 ROM / 319 draft (no change: not
    this decision, or not alone)`); sub_08176F70's suggested CSE_NOCANON knobs did nothing, which
    the worker found by hand. Reload what-ifs are concrete now (below), so they are scored too.
  * Reload rotation (the most-hit gap): `why` on a reload register prints the spill set as the
    choice saw it and the concrete `AGBCC_SPILL=K:D` with the register the ROM's previous choice
    must have taken, or names what rules the ROM's register out (a live pseudo, another reload,
    pass-0 sharing, not a spill register: the "secondary scratch" case where 10:3 "did nothing"
    because D is mod the spill count). sub_081C6780's `15:D`, where 15:1 did nothing, now reads
    `15:2 ... previous choice took r0 or r4, not r2`. `spill` tries one shift per distinct
    register, differing choices first, counts like the listing and lists only this function's
    pins. Inheritance is logged (`I` records) and shown in `why` and `spill`.
  * `life` on a pseudo flow never sees (an alias cse or gcse removed) prints its sets, uses and
    last use after each pass up to the one that removed it, and what replaced it in each use. Such
    pseudos have no allocation record, so they are named from the C line of their first set, or as
    an inline helper's parameter (`p@FlagFour`, `mode@IsMode`).
  * `why all`'s summary lists causes first, then order differences of insns that occur once in the
    ROM listing, and counts (does not list) moves of insns that occur several times (`movs rN, #0`,
    stack stores), which can be paired with another copy; cse substitutions into one register are
    one line (sub_08176F70: eleven copies replaced by pseudo 63). why-all.json is in that order,
    with `unique_insn` on order items.

* Done: trace summarizer and pseudo names (`why`). On RepeatedPairAttack it
  reproduces in one call what took ~15 calls of raw trace reading.
* Done: order. `why all` compares each hunk with low registers blanked; an
  insn both sides have elsewhere is a move. It names the ROM's anchor, says
  whether expansion order or a pass set the draft's order, and for a constant
  follows it to the C use cse placed it at (e.g. a bitfield store's `and 0`
  mask) and the lines where the ROM's first use must be. Passes without an
  explainer say what the removed pseudo held and whether its set was folded.
  On sub_0808B264 this states in one call what took five variants by hand.
* Done: shape distance (register-blind insertions + deletions). `try` ranks by
  it; insn counts ranked the step that led to sub_0808B264's match as worse.
* The allocation explainer's "who held the register" scan is linear over
  the pseudo's insn span, not true liveness (`life` has the true liveness;
  use it there next).
* Done: a `vs OTHER.c` mode: allocation order, priorities and lives of named
  pseudos in two drafts side by side. Done by hand twice this session; it is
  the usual question after an edit fixes one thing and breaks another.
* Done: `why all`. It skips branches that only land elsewhere and says when an
  insn exists only in the final assembly or differs in shape, not registers.
* The `spill` result says which register the ROM's previous choice took, not
  which C adds or removes the reload choices in between: next is a search over
  that span's C (the values in high registers it lists, pins there) for the
  edit that changes the count by the needed amount.
* For gcse-born differences, automatically test bucket order with a few
  `AGBCC_GCSE_T` values (see below) and say whether order alone explains it.
* Done: `try` (whole-file variants or one knob each, through the fork server; sub_0801A6D0's 32
  historical drafts score in 1.3 s, a 13-value knob sweep in 0.23 s).
* Pointers from each decision to the agbcc source function that makes it.
* Global allocation in big functions (sub_080070D0, 564 bytes, landed
  NONMATCHING at shape 58): `why` names the register each pseudo lost, but
  which C change moves a whole allocation order is still a manual search.
  Next: a lever search that tries the known allocation levers (declaration
  order, hoisted pointers, block-scoped temporaries) and ranks them by shape.
* `tools/check.py` run under the venv python (scope re-execs into it) skips
  the per-draft fuzzy score; land calls it with the system `python3`.

* `frame` pairs ROM and draft stack slots by voting over aligned rows (the LCS alignment
  above, 2026-10-26); where whole blocks differ a slot can still pair with the wrong
  draft offset (sub_0821C8F8's quadrant objects). The ROM has no uids, so a better
  pairing needs the accesses' C objects on the draft side matched to the ROM's
  access pattern (which offsets each block reads), not row votes.
* `hoist` explains the draft side only. When the ROM keeps an invariant in the
  loop, the tool cannot tell which "stays" reason the original source used.
  Open case: a table used in one basic block (sub_081A0EA4 and the other
  register-pinned random-table loops); cse deletes a repeated set there, so the
  dead-store lever below does not apply.

## agbcc facts established with scope

Each was read from the compiler source and confirmed on a live draft.

1. **gcse inserts at the end of a block, directly before the cc0 setter.**
   `insert_insn_end_bb` (gcse.c) puts a PRE insertion before the jump, and
   before the `cmp` that sets cc0 for it. So a gcse-made copy always lands
   after any C-written copy in the same block. Seen in
   Vanargand_UpdatePartGroundStrike (`adds r4, r1, #0` born in gcse, after the
   draft's explicit `enter = &p->enter`; the ROM has the opposite order).
2. **gcse never treats a constant as an expression.** `want_to_gcse_p`
   rejects REG, SUBREG, CONST_INT, CONST_DOUBLE, CALL. A struct offset that a
   Thumb add cannot encode is loaded into a fresh pseudo at each use
   (`(plus p (reg K))`), so two `p->field` accesses with a pool-loaded offset
   (e.g. 0x16F) are different expressions to gcse and are not shared; an
   offset built by `movs; lsls` (e.g. 0x160 = 0xB0 << 1) stays inline as
   `(plus p (const_int 352))` and is shared. combine folds the constant back
   only after gcse. This explains the `register u32 off asm("r2") = 0x168`
   pins across the Vanargand part family.
3. **PRE insertion order is expression index order**; PRE reaching registers
   are numbered in hash-bucket order, `hash % T` with
   `T = (insns at gcse / 2) | 1` (docs/BIG_FUNCTIONS.md, "PRE numbering").
   T depends on the size of the whole function before gcse, so a change
   anywhere can reorder PRE temporaries everywhere. `AGBCC_GCSE_T` forces T to
   test whether a residual is bucket order alone; it is a diagnostic, not a
   build setting (the ROM was built with the stock rule).
4. **cse makes the longest-lived register canonical.** `make_regs_eqv`
   (cse.c): a new pseudo becomes the class head if it lives past the current
   extended basic block and its last use is later than the head's. In
   Vanargand_UpdateRepeatedPairAttack the draft reuses the variable `result`
   as "zero" in four stores, so on the path through `if (p->pending)` cse ties
   the `p->timer` load to `result`, and `result` (last use cuid 303) wins over
   the load (182). Writing literal zeros instead removes that tie but makes
   things worse (5 hunks), so the ROM's `str r5` really stores `phase`.

5. **Byte stores into struct fields expand as a bitfield read-modify-write.**
   `p->phase = 1` (u8 member) becomes `t = *addr; t = (t & 0) | 1; *addr = t`
   at expand, even for `struct { u8 phase; }`; an array element store
   (`p[0xA2] = 1`) does not. combine reduces it to a plain `strb` later, but
   cse1/cse2 see `t & 0` as a known zero and enter a fresh register into the
   zero class at every such store. Expect u8 field stores to perturb which
   register cse uses for 0 nearby.
6. **cse's equivalence classes: order decides substitution.** `insert` keeps a
   class in cost order and puts a new member *before* existing members of
   equal cost (only strictly cheaper members precede it). `cse_insn` prunes
   the written source if it is already in the class, then takes the first
   valid member of the cheapest cost ("prefer items not in the hash table" only
   applies to sources not in the class). `record_jump_cond` on `x == C` calls
   `merge_equiv_classes(class(x), class(C))`, re-inserting C's members into
   x's class, so they land in front of x. A register merged at a jump almost
   never wins against registers already known equal to the constant.
7. **Canonicity is re-decided in cse2.** After cse1 substitutes a known-zero
   register for literal zeros further down, that register lives longer, so in
   cse2 `make_regs_eqv` can make it head over a variable cse1 kept. A fix that
   makes cse1 right can be undone by cse1's own substitutions elsewhere.

8. **Reload hands out spill registers round-robin, per function.**
   `reload()` resets `last_spill_reg` to -1 for each function; nothing
   carries over between functions. The spill list is every hard register
   reload uses anywhere in the function, ascending (e.g. r0 r1 r2 r3 r7).
   `allocate_reload_reg` (reload1.c) searches from the position after the
   previous choice, skips registers live there or taken by another reload of
   the same insn (a first pass prefers sharing a register already reloaded in
   this insn), and sets `last_spill_reg` to its pick. Inherited reloads (value
   already in a register) and reloads that reuse the insn's own output
   register make no choice. So the register a reload gets depends on the
   previous choice anywhere earlier in the function. The C-level nudges add
   or remove reloads before that point:
   * a value in a high register (r8-r10) used by a low-register insn: one
     reload per use;
   * a variable that gets no register (long-lived and low priority, e.g. a
     constant set before a loop and used in it): reload rematerializes the
     constant at each use, one choice each;
   * an integer address or a large offset written as a constant
     (`0x0203B400`, `RANDOM_TABLE_ADDR`): Thumb `add` cannot encode it, so
     reload loads it, one choice; the same address as a symbol
     (`gUnk_0203B400`) becomes its own pseudo at expand and needs none;
   * a `register asm` pin: removes the reload of that value.
   `AGBCC_SPILL=K:D` reproduces a shift; `scope spill` searches for one and
   lists the pins, high-register values and register-less values between the
   two choices.
9. **cse's extended blocks skip around blocks without labels.** With
   `-fcse-skip-blocks` (on at -O2), `cse_end_of_basic_block` treats a
   conditional branch over a block with no CODE_LABEL inside as AROUND, so
   knowledge from before the block (e.g. `result == 0` after
   `if (result) return;`) survives past it, into the switch cases. Paths hold
   at most 9 branches (`PATHLENGTH` 10). Each insn is looked at once per
   extended block containing it; the first look that substitutes rewrites the
   insn, so later looks see the substituted form. Unused labels are deleted
   before cse1, so an empty label or `do {} while (0)` does not stop a skip.

* **Solved: Vanargand_UpdateFacingPairAttack** (plain C, no pins). Depinning
  `RandomValue` left the table base in r1 (ROM r2) plus the same one-step
  rotation. One fact explains both: in the ROM the table address is a
  reload. Written as the integer `RANDOM_TABLE_ADDR` (as other matched files
  do), reload loads it as choice #10 -> r2, and `&p->pair`'s constant moves to
  #11 -> r0, as in the ROM.

10. **Constant registers get half priority.** `update_equiv_regs`
   (local-alloc.c) turns a REG_EQUAL note on the single set of a register
   into REG_EQUIV when the value is a constant (a symbol address or a number)
   and doubles that register's `REG_LIVE_LENGTH`; its comment says this is to
   lower its priority. Global priority is
   `floor_log2(refs) * refs / live * 10000`, so every pointer to a table, a
   constant mask or an address constant enters allocation at half the
   priority its uses suggest. cse gives such registers the REG_EQUAL note even
   through copies (a gcse copy of a symbol load, a local `table = gSym`, an
   inline parameter), so writing the pointer differently does not escape it.
11. **Register preferences spread along value chains.** `expand_preferences`
   (global.c) merges preferences across every insn where one pseudo dies and
   another is set (and they do not conflict). In
   Hresvelgr_SetCosineScaledVelocity the table copy's copy-preference for r3
   flows 97 -> address -> loaded value -> product, and at the last multiply
   into `speed`, which is allocated first and takes r3.
   **The REG_DEAD note order decides who receives it.** The loop walks an
   insn's notes in order and each dying value swaps preferences with the set
   value, so a value whose note comes after another's also receives what that
   one brought in. At `(set 84 (mult 82 48))` with notes `82, 48`, `speed`
   (48) gets the r3 that 82 brought; with notes `48, 82` it gets nothing.
   flow writes the notes in operand order (it walks operands last to first and
   prepends), so `radius * table` instead of `table * radius` reorders them;
   combine rewrites the notes of an insn it changes (`distribute_notes`), which
   is why the s16 variant (an extension folded into the multiply) had no leak.
   The source operand order survives to RTL here because neither operand is a
   constant (`expand_binop` only swaps a constant or a non-register first
   operand).
12. **A modulo or division hands its destination to the quotient.**
   `expand_divmod` (expmed.c) uses the assignment's destination as the
   quotient's register unless the destination appears in an operand or has
   another mode. `x = t % 512;` computes the quotient in `x`, then the
   remainder into `x` again; as a subexpression (`t - t / 512 * 512`, or a
   `%` inside a call's arguments) the quotient gets a fresh register. An
   inline helper also avoids it, but integration substitutes known constants
   into their uses (`subst_constants`, integrate.c), so the 511 rounding
   constant is no longer loaded into a register first: one RTL insn shorter,
   which changes the lives of everything live across it.
13. **Reload evicts and re-places pseudos.** When reload needs a register for
   its spills it evicts the pseudos global alloc put there and re-places
   them with `retry_global_alloc` -> `find_reg`. Pass 0 only reuses a
   register already in use that no conflicting pseudo prefers; pass 1 takes
   the first free register from r0 to r15 (Thumb has no `REG_ALLOC_ORDER`).
   The G record shows global alloc's choice and the dispositions show the
   final one; when they differ, the free register set at that point decides.
14. **`regs_may_share` only comes from loop.c**: an invariant moved out of a
   loop may share an allocno with the register it replaced. Outside loops
   every pseudo is its own allocno.
15. **Inline parameters are copied unless read-only.** `expand_inline_function`
   copies each argument into a new pseudo unless the parameter is `const`
   and the argument a register; the copies of `v`, `angle`, `radius` are what
   gives an inlined `polar(&actor->velocity, ...)` its late-born `radius`.
   (In the cosine twin, `const` changed nothing: the argument is a ternary's
   result, which is copied anyway.)

16. **Pass 0 of `find_reg` only reuses registers already in use.** With no
   preference of its own, a pseudo first looks at `regs_used_so_far` (the
   call-clobbered r0-r3, registers live in the RTL, local alloc's choices and
   everything global alloc gave out before it) minus its conflicts and minus
   `regs_someone_prefers` (preferences of conflicting pseudos allocated
   later). Only if that is empty does pass 1 take the lowest free register.
   So a pseudo's register depends on pseudos it does not conflict with: r5
   is reachable in pass 0 only after some earlier pseudo got r5. In the
   cosine twin, `actor` (r5 early) and `speed` (r5 later) never overlap; the
   ROM allocated `actor` first, the draft allocates `speed` first (1250 vs
   1153), finds {} in pass 0 and takes r3 in pass 1. The instrumented
   compiler's F record (one per `find_reg` call, in the AGBCC_DUMPPRI file)
   logs these sets; `why` explains from it.

17. **flow measures lives before combine.** The pass order is cse2, flow,
   combine, regmove, lreg: REG_LIVE_LENGTH and REG_N_REFS come from the RTL
   as expand and cse left it. RTL that combine later folds away (a type
   conversion's extension whose result is known narrow, a copy combine
   merges) still lengthens every value live across it. That makes parameter
   and variable types a lever on allocation order that leaves the
   instructions unchanged: in the cosine twins an `s16` parameter added 6 insns
   to `speed`'s life and 2 to `actor`'s, which flipped their order.

18. **combine drops a redundant mask only for a register set once.**
   `nonzero_bits` of a pseudo is tracked only when it has a single set, so
   `angle = (a + b) & 255;` lets combine delete a later `angle & 255` (an
   inline's index), while `angle = a + b; angle &= 255;` keeps it. In
   Hresvelgr_GetTravelTarget the ROM masks twice, so the original used the
   second form.
19. **fold reassociates constants within one expression.**
   `(x << 6) + (y % 32 + 112)` is computed as `((x << 6) + 112) + y % 32`;
   a variable (`s32 offset = y % 32 + 112;`) is a boundary fold cannot see
   through. Keep statement order as the ROM's evaluation order when you split.

20. **Constant registers and a second set: order matters.** update_equiv_regs
   (local-alloc.c) walks the insns in order. At a set whose value is an
   invariant (REG_EQUAL constant) it accepts a multiply-set pseudo, turns
   the note into REG_EQUIV and doubles its live length on the spot (half
   priority, fact 10). A later set to a non-constant drops the equivalence
   (`no_equiv`) but does not undo the doubling; a non-constant set *first*
   marks the pseudo `const0_rtx` and the constant set is then refused before
   doubling. Checked on a two-line file: constant then `t = p + 9`, 7 -> 14
   insns; `t = p + 9` then the constant, 7 -> 7. In
   Hresvelgr_SetRandomTravelTarget `u32 t = (u32)gUnk_0203B400; ... t = base
   + offset; t &= 255;` matches with no pin although the constant set comes
   first: what reusing the variable bought there is the refs of both values
   on one pseudo (more refs, higher priority), not the end of the doubling.
   `why` reports which case applies (it checks the REG_EQUIV note after
   local-alloc) and asks for the non-constant set before the constant one.
   ContactActorPool_Start's worker showed the earlier version of this fact
   (and of `why`) wrong: `table` set twice, no REG_EQUIV note, life doubled.

21. **Loop-invariant motion (loop.c) and how to keep a set in the loop.** An invariant
   set moves when `threshold x savings x lifetime >= insns in the loop`. The threshold
   starts at 13 in a loop with a call (26 without) and drops by 3 after each hoist, and
   loop.c runs twice (rerun-loop-opt), starting again at 13. Being late in the first pass
   does not keep a long-lived invariant in: the second pass moves it. A set never moves
   if it is set in two blocks of the loop or again after a use (may_not_optimize), or if
   it follows a branch in the body (or is read before it) while its uses span blocks.
   EffectManager_SpawnParticles: the ROM loads the random table at the top of each pass
   and keeps it in r4 through seven draws. `table = gUnk_0203B400;` at the top, plus the
   same store again, dead, at the end of the body (past the join labels, where cse
   cannot prove it redundant), makes it may_not_optimize. Flow deletes the dead store,
   and the function matches with no pins. In one basic block cse deletes the repeat, and
   the lever does nothing. `probe` found it (`AGBCC_NOMOVE=271: MATCH`); `hoist` shows
   the arithmetic and the reasons. EffectManager_SpawnParticlesAt (the twin, whose table
   uses sit in one block of a multi-block body) matched with the same line on its first
   try. `hoist` now writes and scores that variant for every hoisted variable
   (`lever: ... MATCH`). It needs a C variable to re-set: when the hoisted value is an
   unnamed constant inside an inline, name it at the top of the body first.
22. **Stack slot order is pseudo order.** Reload gives each spilled pseudo a slot in
   pseudo-number order (alter_reg), at rising offsets; pseudos it evicts later come
   after. Pseudo numbers follow first appearance, so declaration order is a frame lever.
   `frame` prints the order the ROM needs.
23. **Small forms seen in EffectManager_SpawnParticles.** `if (x) f = 1; else f = 0;`
   gives the store-flag `(x | -x) >> 31`; `f = x != 0` branches. `p->s8 = v | -128`
   folds to a QImode constant; computing it in an `s32` temporary keeps it SImode, so
   reload's move2add reuses -128 for the next `adds r1, #0x7e`. A QImode subreg operand
   goes first in an AND, and a `(u8)` cast narrows it. For a mask-first `ands`, write
   `1 & a->flags`. A local pointer variable (`sprite = &p->sprite`) creates the
   DEST_REG giv that strength reduction steps; the same address written inline does not.
24. **A pointer used only in adds of sp-encodable offsets prefers STACK_REG.** Thumb's
   addsi3 has an `add rd, sp, #imm` alternative (multiples of 4 up to 1020), so regclass can
   price sp cheapest for a pseudo whose only uses are `p + 0x1E0` and `p + 0x5C`. STACK_REG is
   sp alone, so find_reg falls to the alternate class and a value that does not cross a call
   lands in ip (r12, call-clobbered, first free). `why all` now reports such a pseudo when the
   ROM never uses its high register ("class: ... prefers STACK_REG and sits in ip"). Fix in
   Slime_EmitDeathDebris: one `u8 *base` that is the actor and then, after `base += 0x5C`,
   the position, so the in-place add and the call arguments give it a low class.
25. **Zeros between a halfword AND and its store come from a bitfield store.** ROM
   signature: `ands rA, rB; movs rX, #0; [movs rY, #0;] strh rA, [..]`. Later byte and word
   zero stores reuse rX/rY, and the second zero often takes r4 and moves a pointer up one
   register. Typed zero locals cannot give two zeros: cse1's `make_regs_eqv` merges every
   pair (128 type and order variants tried). Clearing through a bitfield view does:
   `struct ChaseFlags40 *f = (void *)&x->flags; s32 mask = ~0x40; f->bits &= mask;`; its
   store expands with the zeros exactly there. A `u16 value : 16` view does the same for
   `++timer->value`. This depinned Vorn_UpdateApproachRange, Vorn_UpdatePursuitWait and
   Vorn_RestoreRouteMovement; about 110 functions show the signature, and a pinned one should
   try this first.
26. **A statement macro doubles the refs inside it.** flow adds the loop depth to REG_N_REFS
   per mention, and a `do { ... } while (0)` with a body emits loop notes, so every mention
   inside counts twice. Armor_UpdateAlignedAttackDecision's pins on dz (r5) and az (r2) were a
   priority swap (dz 12 refs/131 lost r5 to the mode bit's 10/76); one macro around the
   delta, grid rounding and magnitude statements gave dz 18 and az 8 refs and matched. `why`
   now names the narrowest line run that would flip such a swap ("a statement macro ...
   around lines A-B"). The macro must be one the original plausibly had, shared by several
   functions (the Armor twins' ARMOR_PLAYER_DELTAS); a wrapper used at one site, written
   only to double its statement's refs, is a priority knob and counts as a barrier
   (tools/statement_macros.py; progress, land and matchsearch count it). Its reload rotation was a separate fix: a tail that cross-jumping
   later merges still takes its reload choices first, so writing out the duplicated
   `p->decision = 1; ...; break;` tails the ROM merged set the rotation.
27. **A goto into a shared tail hides the tail's reload choices.** jump2 cross-jumps after
   reload, so the ROM's `b shared_tail` may come from a tail the original wrote out on that
   path: the copy makes its reload choices (constants, addresses) and only then is merged.
   A decompiled `goto` skips those choices and shifts the rotation for everything after it.
   Writing the tails out removed rotation barriers from sub_0818911C (the mode branch's
   fastMode/secondaryCooldown choices) and Armor_ApplySceneCommands (each case's own
   Trigger_ConsumePending). `why` on a rotation difference now lists the gotos before it.

28. **`movs rA, #1; rsbs rA, rA, #0; adds rB, rA, #0` is -1 reloaded into a QImode pseudo.**
   movqi takes only constants 0..255, so reload puts -1 in a SImode spill register and copies
   it. A local s8/u8 is promoted to SImode (a direct -1, or #0xff), so the pseudo is the
   temporary store_field makes for `p->field = c ? 1 : -1;`. The same conditional stored
   through a cast pointer, `*(s8 *)(p + 0x1DA) = ...`, takes store_expr's COND_EXPR-into-MEM
   path and stores in each arm. Armor_InitPartStretch, Armor_BeginPartTravelToPlayer and
   Armor_RetractPartStretch matched or moved this way; `why all` now names the pattern.

## Survey: random table as a literal address (2026-10-24)

`SCOPE_TRANSFORM=tools/scope/transforms/random_table_literal.py` over every
file that reads `gUnk_0203B400`: of 207 matched files, 170 compile the same
and 37 change, many of which already use a local `table = gUnk_0203B400`,
so the original was not one universal spelling. Over the drafts: 27 better,
51 worse, 38 the same, 1 match. Large wins left to look at: sub_080B2EEC
18 -> 4, sub_081FF454 8 -> 4, sub_08137E28 69 -> 38,
BossSlot2_SpawnBobbingFormation 10 -> 1. Matches that use the literal:
#642, #643 (SceneHud_SelectUpgradeableStat), #644
(SceneEffectSchedule_AddFromScript, with a `scaleUnlessFlagged` inline whose
if/return leaves a label that stops reload's move2add).

## Survey: how much of Bosses is reload rotation (2026-10-24)

`scope spill` on all 60 NONMATCHING boss drafts (22 skipped: over 40
differing insns). Of the 38 searched:

* 2 match exactly by rotation alone: Hresvelgr_InitializeShots
  (`AGBCC_SPILL=5:3`) and Vanargand_UpdateFacingPairAttack (`10:1`).
* 5 shrink a lot: BossSlot2_SpawnBobbingFormation 12 -> 2,
  Vanargand_UpdatePartSweepingStrike 33 -> 10, Nidhoggr_UpdateFallingAttackEffect
  42 -> 10, Vanargand_UpdatePartSidestep 11 -> 5, EffectPool_UpdateEntry 10 -> 6.
  Rotation is one of their causes; `why all` after applying the shift shows
  the rest.
* 13 shrink a little (1-15 insns), 18 not at all (rotation is not involved).

The search is greedy (one shift per reload, kept if it helps), so "not at
all" is a strong no and "a little" is a lower bound. A rotation shift is a
diagnosis: the C still has to produce the ROM's reload count. In FacingPairAttack
the candidate is line 106: `adds r0, r4, #0; adds r0, #0xa4` is born in reload
yet logs no spill choice (reload reused the insn's own register). If the ROM's
compiler allocated it through the rotation, the state would be one step on,
which is exactly `10:1`. Not tested.

## Live investigations

* **Solved: Hresvelgr_UpdateBoundaryBarrage** (#647, two pins fewer). Scope's
  `why` on the two differing insns said the quotient of `(rawX >> 3) % 512`
  was computed in `x` itself; expmed.c's target rule (fact 12) gave the
  fix, `t - t / 512 * 512`. The helper alternative fixed the quotient but cost
  `px` one insn of life (`life px`: 29 -> 28, the 511 constant folded), which
  moved it ahead of `target` (priority 714 vs 697) and rotated r8/sb/ip.
  With the modulo fixed, the SetHeading pin is unnecessary. Remaining
  concessions, both diagnosed: without the `state` pin, `state` (10 refs,
  life 41) outranks the flags pointer (9 refs, life 44); the pinned
  SetHeading adds exactly the 8 insns (`life state`: 41 -> 49) that flip it,
  via a temporary, a copy and an empty asm per half. Four natural SetHeading
  forms (one expression, inline quantize helpers, a temporary per field) all
  leave `state` at 41. The `aoff` keep-alive: reload evicts the 0x3FF mask
  from r5 and re-places it (fact 13); in the ROM sl is taken because `aoff`
  is live during the mask's life, in the draft it is not (only case 2 reads
  it). Open: what keeps `aoff` live through case 1 in the original.
* **Solved: Hresvelgr_SetCosineScaledVelocity and Hresvelgr_SetSineScaledVelocity**
  (sub_0819C8F8 #648, sub_0819C870 #649; plain C, no pins). One value in r3
  instead of r5, found by `why` in two steps:
  (1) `speed` inherited the table copy's r3 preference at the z multiply
  through the REG_DEAD note order (fact 11); `radius * table` fixes it.
  (2) Then `speed` took r3 in pass 1 of `find_reg` (fact 16): r5 was not in
  use yet because `actor`, which the ROM also keeps in r5, was allocated
  after it (1153 vs 1250). `AGBCC_PRI=22:+98` confirmed it (MATCH), and the
  explainer's arithmetic said `speed` needed 27+ insns of life (it had 24).
  polar's `angle` as `s16` gives that (fact 17): the conversion's RTL counts
  toward flow's lives and is folded away by combine, so the output is
  unchanged except for the allocation (`vs`: speed 24 -> 30, actor 26 -> 28,
  actor now first). Both changes are needed. The original draft had already
  tried `s16 angle` (with an `s8 radius` and odd z index), so the shape was
  close; what the tool added was knowing which property to aim for.
  Without the tool, `const` parameters, the matched siblings' helper idioms,
  a named `speed` local and other operand orders were all tried with no effect.
  The sine twin also needed its own inventory start: gbadisasm left it as a blob.
* **Hresvelgr_GetTravelTarget** (sub_0819C5D4): 8/6 -> 3/3 in plain C
  (`build/scope/drafts/gtt_best_natural.c`, not landed). Facts 18 and 19 fixed
  the `+ 112` placement and the double mask. Left: (a) the ROM loads the
  random table's address before the index update: a `u32 table =
  (u32)gUnk_0203B400;` first in the helper places it, but then the table
  (constant register, half priority) loses r4 to the 1023 mask constant
  (576 vs 789); `AGBCC_PRI=26:+214` leaves only (b); passing the table in as a
  helper argument changes nothing. (b) The ROM copies `out` (r1 -> r5)
  before `actor` (r0 -> ip); RTL generation emits parameter copies in
  declaration order and an inline wrapper with `(out, actor)` does not change
  it. No scope explainer covers either: they are decided at RTL generation.
* **Hresvelgr_SetRandomTravelTarget** (sub_0819C6AC): 9/9 -> 2/2 in plain C
  (`build/scope/drafts/srt_best_natural.c`, not landed). Facts 18 and 19 as
  in GetTravelTarget, plus `add`'s source loaded into a temporary first
  (`target = saved->target;`): the ROM loads it before computing
  `&actor->travelTarget`. With the table-first helper the only cause left is
  the shared one, table vs the 1023 mask (`AGBCC_PRI=26:+214` -> MATCH). The
  levers scope gives: the table with a 4th ref, a life of 18 or fewer (has
  26), or not a constant register (then 1153 > 789). Tried without effect:
  `const u16 *table` local, `table + offset` order, `table += ...` (the
  table set twice kills REG_EQUIV but costs a second load), a named index.
  A `register asm("r4")` pin on the table makes it worse (4/6). Solving this
  helper likely solves both siblings and maybe other callers of the random
  table helper.
* **Solved: Hresvelgr_InitializeShots** (moozilla/boktai3-decomp#641, plain C,
  no pins). The hypothesis below was right except for the constant: it is a
  pre-loop variable `x = 0x708` assigned before `y` and `z`, so it lives
  longest, gets no register, and reload rematerializes it in the loop (choice
  #5 `=1800` -> r0, then `y` from sl -> r1, `z` from sb -> r2, the old #5 -> r3
  as in the ROM). Assignment order matters, declaration order does not.
  Original notes: 4 insns. MATCH with
  `AGBCC_SPILL=5:3`: everything is reload rotation (fact 8). Reload #5 (line
  64, `zero` from r8) gets r1, the ROM r3, so the ROM made extra reload
  choices before it. The likely spot is the inlined `SetPosition`: the draft
  pins `y`/`z` to r1/r2 with `register asm`, so they need no reload. Removing
  the pins turns them into reloads (r0, r1), but the ROM's state is one choice
  further on (r1, r2, then r3), which fits the constant 0x708 being a reload in
  the ROM too. In the draft it is a pseudo from expand (Thumb `movhi` forces
  stored constants into a register), so that would need the pseudo to miss a
  hard register. Open.

* **Vanargand_UpdatePartGroundStrike** (src/fn/sub_081B28D8.c, 1 insn). ROM
  copies `&p->flags` (gcse copy) before `&p->enter` into r8; the draft's
  explicit `enter` pointer comes first. Dropping the pointer loses r8 reuse
  entirely (fact 2). Open: what C makes `&p->enter` shared yet copied after
  the gcse insertion.
* **Vanargand_UpdateRepeatedPairAttack** (src/fn/sub_081AA178.c, 1 insn). ROM
  stores `phase` (r5) to `p->timer` in case 0 (line 76); the draft stores the
  call result (r7). `CSE_KEEP=24` (keep `phase` canonical) matches exactly, so
  this is cse canonicity (facts 4, 6, 7). Chain in the draft: at the switch
  compare `phase == 0` the zero class (holding `result`) merges into phase's
  class; `result` lives longer (used as a zero at lines 96 and 104), so
  `make_regs_eqv` makes it head. Literal zeros at 96/104 move the takeover to
  cse2, because cse1 substitutes the known-zero call value there anyway. The
  fully natural form (no `result`, literal zeros, no asm) gets cse1 right
  (`phase`) but cse2 wrong, and differs in 5 hunks (9 insns).
  The ROM's facts: literal `movs r0, #0` at line 88, r7 at lines 96 and 104,
  r5 at line 76, and `phase` reused as the constant 1 in case 1. Consistent
  story: cse1 does not know "call == 0" in cases 1 and 2, cse2 knows it at
  96/104 but not 88. Refusing cse1 jumps into the cases
  (`CSE_NOFOLLOW_JUMP=...:1`) does not reproduce it (it loses other
  knowledge too). Open: what C shape limits cse1's knowledge that way
  (label use counts at cse1 time, mode of the tested value, switch shape).
  With `why` (literal zeros at 96/104): cse1 is right (`phase` heads the zero
  class); in cse2 pseudo 50 (the zero-extended call value, the ROM's r7) takes
  over because cse1 substituted it at line 104 (last use) while `phase`'s last
  use is line 95 (cse1 substituted it for the literal 1). cse1 knew 50 == 0 at
  line 104 because every look at it is in an extended block from before
  `if (result) return` that skips AROUND the `if (p->pending)` block (fact 9):
  path `21A 102A 620N 623T 631T`. Blocks starting after the pending block do
  not know it. So the ROM's cse1 most likely could not skip that block, or
  did not reach case 2 on such a path. Not the path length (5 of 9).

## Paper cuts found in older tools

* step.py misreads functions with jump tables (reported 35 differences where
  there are 2) and takes 8 s.
* check.py prints raw hex when run outside the venv (no capstone).
* agbccsrc.py keyword lookup returns unrelated sections.
* `tools/worktree.sh` does not build scope's compiler in new worktrees: run `sh tools/scope/build_agbcc.sh` once.
* scope itself, fixed: `FILE.c insn UID` used to treat `insn` as the function
  name; high registers (sb/sl/fp/ip) were not parsed, which mis-paired ROM and
  draft operands; positional operand pairing reported reload registers as a
  pseudo's home (dispositions now win); "allocated N of M" counted the wrong
  list; the rival explanation claimed an order without checking it.
* scope gaps found on GetTravelTarget: no explainer for expression shape
  decided by fold/expand (reassociation, statement order, parameter copy
  order); `why`'s header line for an inlined insn can name the inline's line
  rather than the caller's; `insn UID` only shows passes that changed the
  insn, so "rtl ... combine (gone)" hides the passes in between.
* First cheap-model trial (2026-10-24): Haiku, given WORKER.md, matched
  Hresvelgr_InitializeRadialOffsets (#650) in about 30 compiles and 5
  minutes. `why` gave the allocation lever (angle needed fewer refs); it found
  the address shape by trying forms. Its complaints, addressed: refs did not
  match any count it could see (now broken down as mentions x loop depth in
  `why` and `life`), compile errors looked like empty output (now
  `COMPILE FAILED` with a C89 hint); open: no shape explainer, no RTL diff
  between two drafts, "no explainer for this pass" for loop and regmove.
* Second trials (2026-10-24), Vanargand_UpdatePartSidestep (868 B, 6 pins,
  11 barriers): Haiku stayed at 9/6 but removed 3 pins for free (by hand:
  hence `pins`); Sonnet, starting from that, reached 5/5 (+3 moved branches)
  with 2 barriers fewer and explained the open `bossArg` r8/r7 question (a
  consequence of where `state` lands; `AGBCC_FORCE=269:7` fixes all but one
  hunk). Sonnet's feedback, addressed: knobs take C names (pseudo numbers
  shift between edits, so a suggested `AGBCC_PRI=428:+871` silently did
  nothing on the next draft); moved branches no longer count as differences;
  gas failing on `asm/macros.inc:1 ... '@'` depending on the `.s` size
  (scope now inlines the macros). Open: an explainer for the copy reload
  makes when cse rewrites an `asm("" : "+r"(x))` input (found via `insn`).
  Takeaway: Haiku is enough for small drafts where `why` names one lever;
  pin-heavy functions need Sonnet or better, and still did not match.
* Codex gpt-6.1-sol (2026-10-24, high effort) on the sidestep: MATCH (#654),
  8 pins / 7 barriers, 76 compiles, 2.07M input tokens (1.999M cached),
  15K output. Path: `why` + `AGBCC_FORCE=bit#2:7` (6 -> 3), `life`/`vs` to
  keep the mask alive through an existing barrier in C, `AGBCC_SPILL=16:2`
  then `AGBCC_FORCE=615:1` for the final address, pins for those, then
  `pins` removed 6 barriers. Its feedback, addressed: `why all`, `pins` and
  the display now print the same `N ROM / M draft (+B moved branches)`;
  `pins` caches identical variants, only tries removals that are free alone
  in its greedy pass (`--all` for the rest; 43 -> 17 compiles here), keeps
  its output under SCOPE_RUN and names the real variant directory. Open:
  explain reload-created copies and output-time splits through their source
  pseudo (insns 1409/1453 had no pseudo or reload record); a concrete lever
  for local-alloc choices (which dying operand it tied to); `life NAME`
  picks an inline helper's variable silently (show every instance with its
  declaration and call site); count the extra compile a named knob costs.
* Sol, second run (phase 3: no new concessions; 114 compiles, 2.86M input
  of which 2.60M cached, 20K output): 15 -> 13 concessions with MATCH (#655;
  `endFlag = 8; endFlag &= flags;` for a regmove operand merge, an inline
  `ScaleSequenceSpeed(weight, speed)` for a local-alloc preference); plain C
  89/75 -> 76/64. Where it got stuck, and the tool gaps: regmove merging
  input into result (losing the ROM's copy; 3 pins) has no explainer; a
  priority the knob confirms (`duration` over `flags`, `state`) found no C
  lever; shape-only differences (an immediate load instead of a mask copy,
  dropped byte extensions) have no confirming knob; misaligned rows can
  suggest misleading register diagnoses; `why all` recompiles per hunk.
* Overnight campaign, label-only drafts (2026-10-24): three queued drafts
  (ContactActorPool_Start, Player_EmitMovementEffects,
  Player_UpdateTetherMovement) compiled to the ROM's instructions with one
  branch landing elsewhere, and scope said that branch "follows from the
  others" when there were none. A worker spent 77 compiles on
  EmitMovementEffects and found the cause by hand (`CSE_NOCANON=50`
  matches). scope now names both targets of a label-only difference with
  their C lines and says loop head (an invariant hoisted in one side only)
  or jump threading (a branch onto a repeated test), and `probe` runs
  rtlprobe's canon / follow / pre searches through scope's scoring:
  `CSE_NOCANON=265: MATCH (pseudo 265 set at line 127, made canonical over
  pseudo 270 set at line 127)` for UpdateTetherMovement in 68 compiles,
  10 s. That gives shape-only differences a confirming knob when the cause
  is a cse or gcse decision; differences from combine or fold still have
  none.
  Worker feedback from the second rounds, addressed: `why` on the moved
  branch still said "the code between differs" (it now says the target is
  the whole difference and points at probe; `why all` says there is no
  insn to explain), and probe's C lines were the call site for every
  inlined set (it now names the inline body line and helper: `pseudo 50
  set at line 24 (in FlagFour, inlined at line 57), 25 (...), made
  canonical over pseudo 55 set at line 13 (in Mask, ...)`). Open: both
  Player functions stay one branch off after ~260 compiles between them;
  every C form that keeps FlagFour's `return 0` out of the tested value
  breaks the layout (16-31 insns), so the ROM's helper differs some other
  way.
* Near-miss failures overnight (2026-10-24): of the drafts that started
  within a few insns and failed both rounds, three are reload rotation
  only (`spill` gives MATCH with one shift: Player_GetSmoothedDirection and
  its twin sub_0823C4B4 `AGBCC_SPILL=6:1`, GridAttackActor_OnDamageCollision
  `3:2`); SceneActorControl_UpdateWanderTarget, TickRandomMotion and
  Skeleton_TickDecayingLunge are not. Workers spend 120-190 compiles on
  rotation cases without finding the C that adds or removes one reload in
  the span `spill` names. Open: a search over that span's C (the values in
  high registers it lists, pins there) that reports which edit changes the
  reload count by the needed amount, instead of leaving it to the worker.
  ActorParticles_SpawnWideBurst showed a second reload gap: the ROM has
  `movs r0, #7; mov ip, r0; mov r0, ip; ands r2, r0` and the draft dropped
  the `mov r0, ip` (reload inherited r0's 7 for the `ands`; the ROM's did
  not). It has since matched; inheritance is now logged (`I` records) and
  `why` on an inherited reload names the reload it reuses.
  Its second round matched with one empty barrier after `mask = 7`, which
  confirms the inheritance diagnosis; the plain-C form is still unknown.
  Power_EnterSleep needs an inline `svc #3` (SDK SystemCall), which agbcc
  cannot emit from C: a policy question, not a tool gap.
* Codex gpt-6-luna (2026-10-24, high effort) on Haiku's easy task
  (Hresvelgr_InitializeRadialOffsets, 14/12 from the foundry draft): no
  progress in two runs (11 and 7 compiles, ~0.5M input tokens each, mostly
  cached). It confirmed the allocation diagnosis with the knob (8/6) but
  reverted the shape split because it did not pay off alone, and never tried
  `i * 64`. After run 1 the ref lever lists the counted mentions by line
  (marking shifts) and WORKER.md says to keep changes that do not pay off
  alone; run 2 still stopped early and restored the draft. Luna is not a
  scope worker; Haiku is the cheapest that matched.
* Pin survey (2026-10-24, `pins` over all 878 matched files with
  concessions): 6,303 concession lines (3,976 pins, 2,327 barriers), of
  which 18 were free (landed in #652, one function now concession-free).
  The rest are load-bearing: phase 3 needs a diagnosis per concession. The
  survey found `score()` counting phantom differences when the ROM listing
  misaligns a literal pool (22 files; it now trusts byte equality).
* The shared instrumented compiler was older than its patch (see above);
  the one in the scope worktree also lacked AGBCC_NOPRE until it was rebuilt
  with `tools/agbcc-instr.sh` (150 files, plain and NONMATCHING, compile
  identically to agbcc without knobs).
* scope, fixed: `life`, `vs` and `insn` found no names or records when the
  draft #defines a friendly name to the ROM symbol (records use the ROM
  name; now they fall back to the only function in the dump), and a trailing
  FUNC argument to `life` was read as a variable name; a `sub_ADDR` name that
  the CSV has since renamed raised a KeyError.
