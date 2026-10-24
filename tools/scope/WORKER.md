# Matching a function with scope (worker brief)

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

Other commands:

```
scope.py FILE.c life NAME...        insns that count toward a value's live length, by C line
scope.py A.c vs B.c                 two drafts' register allocation side by side
scope.py FILE.c spill               is the whole difference reload's spill rotation?
scope.py FILE.c pins                cost of removing each register pin / asm barrier; greedy free removal
scope.py FILE.c insn UID            one RTL insn through every compiler pass
scope.py FILE.c uses REG...         which insns use a pseudo after each pass
```

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
* **Reload rotation:** if `spill` says the difference is only reload's
  choices, the fix is upstream (which values got no register, which
  constants reload loads).

## Rules

* agbcc is C89: in each block, declarations come before statements.
  `COMPILE FAILED` means the file did not compile; fix that first.
* No `asm()` statements and no `register ... asm("rN")` pins.
* Keep the C readable: names, struct fields, inline helpers are fine.
* Stop when you get MATCH, or after about 25 compile attempts without
  progress.

## Report

* MATCH or the best count of differing insns (`N ROM / M draft`), and the
  path of your best file.
* For each change that helped: what `why` said and what you changed.
* **Where scope's output was confusing, wrong or missing** (this matters as
  much as the match: it is how the tool gets better).
