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
python3 tools/scope/scope.py insn UID [FUNC]      one RTL insn through every pass
python3 tools/scope/scope.py uses REG... [PASS...] which insns (uid@C line) use a pseudo after each pass
python3 tools/scope/scope.py FILE.c why UID       why insn UID has the registers it has (see below)
python3 tools/scope/scope.py FILE.c spill         is the whole difference reload's spill rotation?
```

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
  priority, refs, live length, calls crossed), who already held the ROM's
  register during its life (hard conflicts come from local alloc and fixed
  uses), or which earlier-allocated conflicting pseudo took it, with
  `AGBCC_PRI`/`AGBCC_FORCE` what-ifs;
* for a reload register (operand not a pseudo before allocation): the reload
  choice number and the rotation explanation, pointing at `spill`.

`spill` greedily shifts reload's spill rotation (`AGBCC_SPILL=K:D`) before each
reload choice from the first differing insn on, keeping shifts that shrink the
diff (8-60 compiles, about 0.1 s each). A MATCH means the C has the right
shape and pseudos and only reload's rotation state differs.

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

Knobs of the instrumented compiler (`tools/agbcc-instr.sh`): `CSE_NOCANON=N`
(new reg N never becomes a class head), `CSE_KEEP=N,...` (keep N canonical),
`CSE_DEBUG=N` (trace `make_regs_eqv` for new >= N), `CSE_PATH=1` (trace each
jump cse follows), `CSE_NOFOLLOW=K` (refuse the K-th follow),
`AGBCC_GCSE_T=N` (gcse hash table size), `AGBCC_PRI`, `AGBCC_FORCE`,
`AGBCC_DUMPPRI`, `AGBCC_SPILL` (local-alloc / reload; see the sources for
syntax). Added in this thread (in `tools/patches/agbcc-instr.patch`):

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

With no knobs set the instrumented compiler emits the same code as agbcc
(checked on 62 files, default and NONMATCHING, after these additions; again
on 80 compiles after the trace markers and `CSE_PATHS`).

**The shared `build/agbcc-instr` in the main checkout can be stale**: on
2026-10-24 it predated the patch's `CSE_PATH`/`CSE_NOFOLLOW`. Rebuild with
`sh tools/agbcc-instr.sh` (20 s) after the patch changes; this worktree keeps a
private build so a rebuild never disturbs other threads. Incremental rebuild
after editing one file: `make -s normal BASE_CFLAGS="-O2 -std=gnu11
-Werror-implicit-function-declaration -Wno-error=incompatible-pointer-types
-Wno-everything"` in `build/agbcc-instr/gcc` (2.5 s); then regenerate that
file's section of the patch (`diff -u --label a/F --label b/F`).

### Speed (measured on an M-series Mac)

| Stage | Time |
|---|---|
| agbcc, plain / with all 14 RTL dumps | 7 ms / 19 ms |
| preprocess (`clang -E` direct; the `cc` shim adds ~8 ms) | 14 ms |
| whole command, warm cache | 0.27 s (was 0.7 s; 0.34 s of that was readelf on the ROM ELF) |

agbcc's own startup is not the cost for units this size, so a fork server
buys little for single queries. Remaining overhead is ~10 subprocesses (as,
ld for the stub, objcopy, readelf, objdump) and two Python startups. Next
steps when it matters: resolve relocations in Python instead of linking
(`tools/agbserve.py` has a `Resolver`), and for parameterized search drive
`agbserve.py`'s fork server without dumps (target ~10-20 ms per variant).

### Gaps (next work on the tool)

* Done: trace summarizer and pseudo names (`why`). On RepeatedPairAttack it
  reproduces in one call what took ~15 calls of raw trace reading.
* `why` has no explainer yet for combine, regmove or gcse changes (it says so),
  and the allocation explainer's "who held the register" scan is linear over
  the pseudo's insn span, not true liveness.
* `why` on the first differing insn of every hunk, as one report (`why all`).
* The `spill` result is a classification, not a fix: next is a view of what
  makes the ROM's reload count differ (insns needing reloads, by C line).
* For gcse-born differences, automatically test bucket order with a few
  `AGBCC_GCSE_T` values (see below) and say whether order alone explains it.
* A `try` mode: N variants of one hypothesis through the fork server.
* Pointers from each decision to the agbcc source function that makes it.

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

8. **Reload hands out spill registers round-robin.** `allocate_reload_reg`
   (reload1.c) starts its search after `last_spill_reg`, the index of the last
   spill register it chose, reset per function. So the register a reload gets
   depends on the previous reload's choice anywhere earlier in the function.
   An earlier insn that needs one more (or one fewer) reload shifts every
   later choice. Hard-register pins (`register asm("r1")`) remove a reload, so
   they shift the rotation too. `AGBCC_SPILL=K:D` reproduces a shift; `scope
   spill` searches for one.
9. **cse's extended blocks skip around blocks without labels.** With
   `-fcse-skip-blocks` (on at -O2), `cse_end_of_basic_block` treats a
   conditional branch over a block with no CODE_LABEL inside as AROUND, so
   knowledge from before the block (e.g. `result == 0` after
   `if (result) return;`) survives past it, into the switch cases. Paths hold
   at most 9 branches (`PATHLENGTH` 10). Each insn is looked at once per
   extended block containing it; the first look that substitutes rewrites the
   insn, so later looks see the substituted form. Unused labels are deleted
   before cse1, so an empty label or `do {} while (0)` does not stop a skip.

## Live investigations

* **Hresvelgr_InitializeShots** (src/fn/sub_081A3554.c, 4 insns). MATCH with
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
* `tools/worktree.sh` does not link `build/agbcc-instr` into new worktrees.
* The shared instrumented compiler was older than its patch (see above).
