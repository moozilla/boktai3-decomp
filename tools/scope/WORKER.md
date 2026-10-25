# Matching a function with scope (worker brief)

## Phase 1: a function with no C yet

`scope.py todo` lists them (ROM order; `todo N prior` puts the ones with
the most matched prior art first). `scope.py new FUNC` opens a workspace in
`build/scope/work/FUNC.c` (under `build/scope/RUN/work/` with `SCOPE_RUN`):
the function's unit with an empty definition where its INCLUDE_ASM was, the
C of a matched twin when one covers at least half of its rare instruction
windows, or an empty definition with the signature a header or a caller
declares. Keep its `// scope: write FUNC here` line: it tells every command
which function the file is about. Then:

1. `scope.py rom FUNC` reads the ROM with calls and literal symbols named.
2. `scope.py like FUNC` lists matched functions with the most similar code
   and what they share; read the closest one's C, and the unit's neighbors,
   for types, struct fields and idioms.
3. `scope.py fields OFF... [STRUCT]` names the include/ structs with members
   at the offsets the ROM reads (it does not see structs local to a .c file:
   grep the callers too).
4. Write C until the calls, loops and branches line up with the ROM, then
   follow the loop below. `land FUNC` (exact) or `land FUNC --nonmatching`
   (an unfinished draft) puts it in the tree; `land --name Prefix_Action`
   names it.

## Phase 2: a draft to a match

You are turning a nonmatching C draft into C that compiles byte-for-byte to
the ROM. scope compiles one function in well under a second and explains,
from the compiler's own traces, *why* each differing instruction came out
the way it did. Work from those explanations; do not try C forms blindly.

## Setup (do this once)

* Work in the scope checkout (the directory holding `tools/scope/scope.py`).
  Run every command from its root.
* Copy the draft to a scratch file and edit only that copy:
  `cp src/fn/sub_XXXXXXXX.c build/scope/drafts/NAME.c`
  Do not edit `src/`, do not commit, do not run `make`.
* The function name is found from the file. If scope asks for it, append it
  (`... FILE.c FUNC`).

## The loop

1. `python3 tools/scope/scope.py build/scope/drafts/NAME.c`
   prints `MATCH`, or the hunks that differ: ROM vs draft side by side,
   the C line, the RTL insn uid and the pass that created it. `!` = differs,
   `-`/`+` = only in ROM/draft, `>` = same instruction, different target.
2. Pick the first real difference (ignore branch offsets: they follow from
   size differences above them). Ask:
   `python3 tools/scope/scope.py build/scope/drafts/NAME.c why UID`
   (or `why all` for the first difference of every hunk).
3. Read the explanation. It names the mechanism and a lever, and usually a
   what-if knob. **Confirm with the knob before changing C**: e.g.
   `python3 tools/scope/scope.py build/scope/drafts/NAME.c AGBCC_PRI=97:+768`.
   If the knob gives MATCH (or removes that hunk), the diagnosis is right and
   you only need C that has the same effect.
4. Change the C toward the property the explanation asks for (a longer or
   shorter life for a named value, a different operand order, a different
   type, a value computed earlier or later). Re-run step 1. Keep a short log
   of what each change did to the count of differing insns.
5. **Changes interact.** A change that fixes what `why` diagnosed can leave
   the count unchanged while another difference hides its effect (a shape
   fix whose hunk is still wrong because of allocation). Keep a change
   `why` asked for unless it makes the count worse, and stack the next one
   on top; `A.c vs B.c` shows what moved. Matches usually need two or three
   such changes together.

Other commands:

```
scope.py FILE.c life NAME...        insns that count toward a value's live length, by C line
scope.py A.c vs B.c                 two drafts' register allocation side by side
scope.py FILE.c spill               is the whole difference reload's spill rotation? (SCOPE_SPILL_MAX=42 compiles)
scope.py FILE.c probe               is it one cse/gcse/loop decision (cse canonical register, jump follow,
                                    PRE, loop strength reduction, invariant hoist)? flips each alone
                                    (SCOPE_PROBE_MAX=150 compiles); "no change" covers only those families
scope.py FILE.c pins                cost of removing each register pin / asm barrier; greedy free removal
scope.py FILE.c put SNIPPET.c       SNIPPET.c (FUNC's definition, its structs/helpers above it) in place of
                                    FUNC's definition, then the score; safer than editing a unit copy by hand
scope.py FILE.c try V1.c V2.c ...   score whole-file variants, best first, with the lines each changed
scope.py FILE.c try KNOB={a,b,c}    one variant per knob value (shell expansion makes the list)
scope.py FILE.c frame               stack slots, ROM against draft, with the C variable in each
scope.py FILE.c hoist               loop-invariant motion by pass: what moved, what stayed, and the dead
                                    re-set lever scored for each hoisted variable
scope.py FILE.c giv                 loop strength reduction: each induction value with its C line, loop.c's
                                    benefit and lifetime, and whether it was reduced, kept or eliminated
scope.py FILE.c insn UID            one RTL insn through every compiler pass
scope.py FILE.c uses REG...         which insns use a pseudo after each pass
```

Compiles are cheap (~12 ms per variant in `try`, `probe`, `spill`, `pins`). When one hypothesis
has several spellings (three declaration orders, two helper shapes), write them as files and `try`
them in one call instead of one compile per edit; keep each variant a test of a stated hypothesis.

Knobs take a pseudo number or a C variable name (`AGBCC_PRI=table:+214`);
names survive edits, numbers do not, so prefer the name `why` prints.
Branches marked `>` whose target moved are counted separately: they follow
from the other differences, do not chase them.

Knobs (what-ifs; they never go into the final C): `AGBCC_PRI=PSEUDO:+N`
(raise a value's allocation priority), `AGBCC_FORCE=PSEUDO:REG`,
`AGBCC_SPILL=K:REG` (reload's K-th choice), `CSE_KEEP=PSEUDO` (keep a
register canonical in cse).

## What usually decides it (details: README.md "agbcc facts")

* **Allocation order.** Global alloc gives registers in priority order,
  priority = floor_log2(refs) * refs / live_length. A value that lives
  longer or is used less goes later. `why` prints how many refs or insns of
  life would flip two values; `life NAME` shows where those insns are.
  Live lengths are measured before combine, so RTL that later disappears
  still counts: a narrower parameter or variable type (`s16`, `u8`) adds
  extension RTL and lengthens lives without changing the output.
* **Preferences.** A value prefers the register of what it is copied from or
  into. Preferences leak across `a = b op c` where b or c dies; the operand
  order (`x * y` vs `y * x`) decides who inherits.
* **Constants** (an address, a mask) held in a register get half priority.
* **Refs** in `why`/`life` are flow's weighted count: every mention adds
  the loop depth (1 outside loops, 2 inside one loop), and scope prints the
  breakdown. To lose refs, remove mentions; inside a loop each counts double.
  `i * 64` and `i << 6` can differ in refs because cse and combine treat a
  multiply and a shift differently.
* **Instruction order** mostly follows C statement and expression order;
  temporaries (`s32 t = ...; p->f = t;`) move a load or computation earlier.
* **Expression shape** (no `why` explainer yet: if a differing insn is "in
  no RTL dump" or `why` says nothing useful, it is shape). fold reassociates
  constants inside one expression: `&p->arr[i]` becomes `(i * 8 + 0x338) +
  p`, `(a << 6) + (b % 32 + 112)` becomes `((a << 6) + 112) + b % 32`. To
  get the ROM's order, split at the point the ROM computes separately
  (`q = i * 8 + (u32)p; q += 0x338;`), keeping the ROM's evaluation order.
* **Redundant masks** (`& 255` the ROM repeats and your draft drops):
  combine drops a mask only when the value's register is set once, so the
  ROM's form is a second assignment (`a = b + c; a &= 255;`).
* **`x = a % n` / `a / n`** put the quotient in `x`; `a - a / n * n` does not.
* **A copy the ROM keeps and the draft lost** (`adds r1, r3, #0` then an op
  in the ROM; the op straight into the input's register in the draft):
  regmove tied a two-address op's output to its dying input. `why` names
  the tie, the conditions it needed, and `AGBCC_NOTIE=UID,...` (every
  inlined copy) to confirm. In C: keep the input alive past the op, read it
  or the result in between, or put a branch between the set and the op.
* **Same code, different branch target** (`0 ROM / 0 draft (+1 moved branch)`): scope
  names both targets. A loop head means an invariant is set inside the loop in one and
  hoisted in the other; a forward branch onto a repeated test means jump threading.
  Run `probe`: threading usually depends on cse's canonical register
  (`CSE_NOCANON=N` matches), which means an inline helper's return value outlives the
  value it was copied from; probe prints the C lines that set both.
* **A loop walks a pointer where the ROM indexes (or the reverse):** probe also flips loop.c's
  strength reduction of each induction value (`AGBCC_NOGIV=UID` keeps it recomputed from the
  index, `AGBCC_GIV=UID` steps it); a hit names the C line. `AGBCC_LOOP_TRACE=1` lists them.
* **Expression shape with no `why` answer:** run `probe` before trying C forms; a
  matching `CSE_NOFOLLOW` or `AGBCC_NOPRE` says which cse/gcse decision it is.
* **Reload rotation:** if `spill` says the difference is only reload's
  choices, the fix is upstream (which values got no register, which
  constants reload loads).

## Rules

* agbcc is C89: in each block, declarations come before statements.
  `COMPILE FAILED` means the file did not compile; fix that first.
* No hand-written instructions in `asm()`, ever. Register pins
  (`register T x asm("rN")`) and empty `asm("" : "+r"(x))` barriers are a
  project-authorized last resort (CLAUDE.md): only where a knob proved an
  allocation decision you could not reproduce in C after real attempts, and
  say so in the report. Try to remove the ones a draft already has (`pins`).
* Keep the C readable: names, struct fields, inline helpers are fine.
* Stop when you get MATCH, or after about 25 compile attempts without
  progress.

## Report

* MATCH or the best count of differing insns (`N ROM / M draft`), and the
  path of your best file.
* For each change that helped: what `why` said and what you changed.
* **Where scope's output was confusing, wrong or missing** (this matters as
  much as the match: it is how the tool gets better).
