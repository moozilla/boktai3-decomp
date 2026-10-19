# Matching big functions: measure, hypothesise, read the compiler

Tags: #matching #tooling

How a cloud worker took `sub_08123194` (8964 bytes, about 4,145 instructions with
its far-branch tails) from a first draft to 19 differing instructions in about
90 minutes, rebuilt as tools any worker can run. The function-specific record is
[notes/cloud-08122DF4.md](../notes/cloud-08122DF4.md). Read this page before
starting any function of 500+ instructions, or when a draft's fuzzy score stops
moving.

There was no blind search. The draft improved by measuring each idea with a
score that does not lie on large listings, testing ideas in small batches, and
explaining every remaining difference from agbcc's pass dumps and source.

| Stage | Differing insns (structural / total) | What moved it |
|---|---|---|
| first draft | 533 insns short | condensed listing read section by section |
| full shape | 4444 structural | duplicated switch cases, kind==0 path in the `else` |
| polarity | 2875 | `if (flag & 0x800)`: the ROM lays out that path first |
| mask | 763 | `shape = (flags & 0xF0) >> 4` |
| anchoring | 274 / 750 | anchordiff cut at calls, so inlined copies align correctly |
| inlines | 231 / 1194 | `Delta()` inline, inline-argument order, `CellAttributes()` |
| read twice | 47 / 201 | one `CellHeight` inline reading `*p` twice, used everywhere |
| liveness | 0 / 19 | `result = 2` after the call, `goto check`, tail shape |

check.py's match verdict is byte-exact and always right. Its fuzzy score is a coarse
whole-listing similarity: the first committed draft (1,192 differing instructions)
already scores 74.8%, and the final one (19) scores 99.6%, so it hardly separates
a good edit from a bad one. (The session reported it as stuck at 8.8%; that does not
reproduce with today's check.py.) On 86 current drafts the fuzzy score and the
realigned count agree to a median of 2 points, so the fuzzy score is fine for
triage. Use the realigned count to steer edits.

## 1. Measure with a score that survives insertions

check.py's side-by-side listing pairs rows by position, so after the first
inserted instruction every later row reads as different, and its byte count
jumps the same way. On a mismatch, check.py now also prints the realigned counts:

    realigned: 201 differing insns, 47 ignoring registers (tools/anchordiff.py build/check/run-...)

`tools/anchordiff.py RUN_DIR` prints the hunks behind those counts. It cuts both
listings at the calls they share (matched by callee, so 16 inlined copies
of one loop cannot be aligned against the wrong copy), aligns each piece
with difflib, and `-s` masks registers and stack offsets. Work the structural
count to 0 first; registers come last. `--range ROM_LO,ROM_HI,YOU_LO,YOU_HI`
restricts it to one region.

## 2. Make the listing readable

A big function is usually a few routines inlined many times. Fold each repeated
run into one line before reading: `tools/condense.py ADDR` finds runs that
recur (registers abstracted) and prints the listing with each run replaced by
`RUN_n  regs`, with each run's first copy printed on top. On 08123194 it finds
the 152 inlined cell lookups and the 8 to 12 copies of each Seek loop, and folds
4,045 instructions into 838 lines in 2 seconds. Then write the C one section at a time, using the calls as
landmarks. `tools/relatives.py ADDR --c` finds matched code for the inlines.

## 3. Test ideas in batches

Write each idea as a small edit and score them all at once:

    tools/variants.py FILE.c ideas.txt --nonmatching [--greedy]

`ideas.txt` holds `OLD` / `===` / `NEW` blocks separated by `---` (several edits in
one block are separated by `+++`; `## name` labels a block). Each variant is
checked and ranked by structural, then total, then bytes. `--greedy`
combines the improving ones. `--decl-swaps` generates every adjacent swap of
local declarations instead. Variants are cheap: check.py takes 1.6 s on a
4,000-instruction function, and 15 variants run in 9 s with `-j 4`.

## 4. Explain what is left from the compiler

When a difference survives every idea, find the pass that decides it.
`tools/rtlprobe.py` compiles one file with the pass dumps in under a second:

* `rtlprobe.py dump FILE.c` writes every pass (`-da`). The `@ N` after each
  instruction in `u.s` is the insn number, so grep it through
  `.rtl .cse .gcse .loop .combine .regmove .lreg .greg` to see which pass
  changed it.
* `rtlprobe.py loop FILE.c` lists loop.c's movables (hoisted or "not
  desirable"). A constant that the ROM keeps in a register across a loop is a
  hoisting decision (rule below).
* `rtlprobe.py gcse FILE.c --scan LO:HI` lists PRE's reaching registers in bucket
  order and how the order changes with the table size (below).
* Allocation-only differences go to [REGALLOC.md](REGALLOC.md) and
  [PERMUTER.md](PERMUTER.md) (`regdiag`, `rasolve`, `translate`). rasolve
  answers "not allocation" when the cause is CSE, PRE or instruction
  selection; that is when to use rtlprobe.

## Rules found on the way (agbcc)

Structure:
* **Block order gives the `if` polarity.** If a large chunk is deleted in one place
  and inserted later, the ROM falls into the other branch first. Swap the
  condition and the bodies.
* **A switch whose cases the ROM duplicates** is written with explicit duplicated
  cases; agbcc does not clone them itself.
* **A shared check reached by `bne far; b other`** comes from an explicit
  `if (found) goto check; goto stop;` with a `check:` label. `break` gets inverted.

Values and liveness:
* **Set a value after the call that only one path needs.** `result = 2;
  if (!Seek(...))` kept `result` live across the inlined loops, so it was spilled.
  `found = Seek(...); result = 2;` matches.
* **Read a byte twice instead of caching it.** `shape = (*p >> 4) & 15;
  height = (*p & 15) << 8;` ties `ands` to the constant as the ROM does. A
  `flags` local ties it to flags. CSE merges the loads either way.
* **C narrows `u16 & const` to HImode** and ties the other way. Return the load
  from a tiny inline (`static inline u16 Attr(i) { return *(u16 *)(p + 2); }`)
  or write `*(u32 *)p >> 16 & 0xC3`.

Inlines:
* **Inline arguments: `&s->field` is materialised last** (EXPAND_SUM). Pass raw
  values, compute derived ones inside the inline, and assign the caller's
  copies after the call, so CSE turns them into the ROM's register copies.
* **A vector difference stored through a stack pointer** is an inline
  `Delta(&out, a, b)`, not field assignments.

Loops and constants:
* **loop.c hoists a constant only if `threshold * savings * lifetime >= insns in
  the loop`.** threshold = `(loop has a call ? 1 : 2) * (1 + non-fixed regs)`,
  which is 13 on Thumb, minus 3 for each movable already moved. The loop pass runs
  twice. A mask like `(x >> 4) & 15` that survives in the ROM but folds in yours
  means the ROM hoisted the 15. More uses of the same constant (for example,
  `u8` narrowing that creates separate QI and SI copies) raise savings.
* **`movs rX, #imm` repeated at each use inside a loop** is a hoisted constant
  that lost allocation and was rematerialised by reload.
* **`adds rX, rH, #0; ands rX, rY`** comes from regmove's backward pass when
  no operand can be tied. A variable set twice (`h = f & 15; h <<= 8;`)
  reproduces the copy.

Other:
* Pin registers only as a diagnostic. Pinning one pointer showed which
  differences were downstream of it, and was then removed.

## PRE numbering: the last few registers

GCSE's PRE (`pre_delete`) creates one reaching register per redundant
expression, walking the expression hash table bucket by bucket. The new
pseudos are therefore numbered in bucket order, and allocation (or, when
spilled, stack-slot order) follows that numbering. The bucket is
`hash(expr) % T`, with T = `(insns at GCSE / 2) | 1`. For `(plus (reg R)
(const_int k))` the hash is `13772 + R + k`.

`rtlprobe.py gcse FILE.c --nonmatching --scan 2201:2701` prints the current
order and, for every odd T in the range, the order you would get. It
recomputes the hash from gcse.c and checks it against every dumped entry. If the
ROM's order needs a different T, the C must produce that many fewer (or more)
insns before GCSE without changing the final code. `--need E1 E2 ...` takes
the order you want (read it off the ROM's stack slots or registers) and lists
the T windows that give it, nearest first, with each expression's raw hash
(a pseudo number inside an expression adds to it one for one).

On 08123194 the ROM's slots give the order 217 < 1907 < 1178 < 1075, and
`--need 217 1907 1178 1075` gives T 2267..2347 (now 2593): 490–650 fewer insns
at GCSE. Brute-forcing the pseudo numbers inside the expressions near T = 2593
found no solution, so renumbering alone does not fix it. Either the original RTL
was much smaller before GCSE, or some of those values were ordinary variables
(numbered below every PRE register) rather than PRE temporaries: 217 < 1907 <
1178 < 1075 is exactly that order. Free T reductions that keep the code
byte-identical: a `u32 Shape(i) { u32 b = *Properties(i); return b >> 4; }`
accessor in CanEnter (−4 per site). The same accessor in Seek, a u8→s32 return
from Seek (−56) and a u32 `CellAttributes` (−60) all change loop hoisting or
jump threading. Declaration order moves T by ±16 at most. This is open.
