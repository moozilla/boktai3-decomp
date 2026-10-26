# agbcc matching tips (pre-scope playbook)

Tags: #matching

Patterns collected while matching by hand before `tools/scope` existed (moved verbatim from the
old docs/WORKER.md, now [history/WORKER_2026-10-24.md](history/WORKER_2026-10-24.md)). scope's own
findings are in [tools/scope/README.md](../tools/scope/README.md), "agbcc facts established with
scope"; the Yu-Gi-Oh! EDS catalog is [external/eds/matching-tricks.md](external/eds/matching-tricks.md).

## agbcc matching tips

* **Use real struct types, not `*(u16 *)(p + 0x20)` casts**, whenever the
  asm indexes arrays inside an object. Leaf functions that start with
  `mov ip, r0` (369 of them) come from struct/array-of-struct accesses like
  `p->elems[i].flags &= ~1;`. With pointer casts agbcc pushes r4 instead. See
  `src/fn/sub_0810C1E8.c`. Structs also fixed ordering problems elsewhere
  (`sub_081FC61C`).
* **Switch / jump tables work**: a plain C `switch` reproduces the table.
  Order the case bodies in the source by their order in the original asm,
  not by value; use `r = N; break;` + one `return r`; list `case 0: case 7:
  default:` explicitly when the table does. `switch` on `s32` gives
  `cmp; beq; cmp; bgt`, on unsigned `blo`. (The worklist JUMPTABLE flag can
  misfire on trailing data; a real one has `ldr; mov pc` and a `.4byte` table.)
* **Branch layout**: the original often has the opposite layout from the
  obvious C. Try testing the branch-taken condition first, `goto` for the
  true cases, `if (a || b) r = 0; else r = 1; return r;`, nested ifs instead
  of `&&`, and sequential early returns in the original's order.
* **Constant load order**: declare the constant as a local first
  (`u32 m = 4;`) to hoist its `movs`; declare it inside a block after a call
  to sink it. To get "mask constant before the ldr", use a static inline
  helper: `static inline u32 tst(u32 *a, u32 m) { return *a & m; }`.
* **Commutative operand order** follows the source: try `(i << 2) + (u32)tbl`
  vs `tbl[i]`, permute OR chains, `t = r; t += x;`.
* **Shadow register + MMIO**: `gShadow = expr; REG = gShadow;` (not chained).
  DMA: `struct Dma { vu32 src, dst, ctl; }` and read `d->ctl` afterwards.
* **Hidden args**: an argument register kept alive across a call, or an
  unused-looking parameter, usually means the callee takes more parameters.
  Declare them. `pop {r1}; bx r1` means it returns the callee's value.
* **Reloads**: if the original reloads a global pointer between stores,
  declare it `u8 *` and use casts; a struct-typed global gets CSE'd.
* **Loops**: descending loops with no pre-check are `do {} while`; where the
  pointer increment sits (`e++` in the body vs. the `for`) moves `adds`.
* C89: declarations at the top of a block only.
* **Per-file compiler flags**: a line `// CFLAGS: -O2` (replaces the default `-O2 -mthumb-interwork`) — e.g. library code that ends in `pop {r4, pc}` was built without interworking.
* **Redundant null check** (`bl f; cmp r0,#0; bne; movs r0,#0`): the function
  returns the address of the struct's first field:
  `if (!p) return NULL; return &p->unk0;` (src/fn/sub_08065C40.c).
* **Materialised bools** (`movs r0,#1; b; movs r0,#0; cmp r0,#0`): put the
  test in a `static inline u8` helper (`return TRUE/FALSE`) and call it in the
  `if`. An `int` result gets jump-threaded away (src/fn/sub_080F9374.c). For a
  `bne; movs 0; b; movs 1` return, `if (x & m) return TRUE; return FALSE;`
  works where the ternary doesn't (src/fn/sub_080FF7AC.c).
* **Clone families**: run `build/venv/bin/python tools/clones.py --port --range START END`
  on your range after a few matches; it copies matched C to identical
  siblings and keeps what `check.py` accepts.
* **Near relatives**: `python3 tools/relatives.py --port --keep` ports the C of
  every matched function that explains 90% or more of an unmatched one
  (registers abstracted, so it also finds clones whose matched sibling came
  later) and keeps the MATCHes; the rest stay in `wip/relatives/` with their
  fuzzy score as starting drafts. `docs/tasks/relatives.md` ranks the
  unmatched functions by how much a matched relative explains.
* **Wrong values in the right shape**: `build/venv/bin/python tools/repair.py
  wip/sub_X.c sub_X` takes the ROM's constants and symbols (field offsets,
  ids, arguments, callees, pool callbacks, a block of fields moved by one
  delta) and puts them in place of the C's, keeping each edit that brings the
  code closer. Run it first on any port or draft that is close in shape; it
  takes seconds. What it can't fix it lists as `ROM only` / `C only`
  instructions: a call, guard or store the C is missing, which is your job.
* **Returns a value it never sets**: a non-void function that falls off the
  end (`u32 f(...) { ...; s->v = 0; }`) gives `pop {r1}; bx r1` with a stale r0.
* **Work in progress** goes in an untracked `wip/` directory
  (`check.py wip/sub_X.c` works); copy into `src/fn/` only on MATCH, so the
  full build never sees a non-matching file.
* Function pointers work: declare the callee (`void sub_081F05D0(void);`) and
  pass `sub_081F05D0`. The literal gets the Thumb bit (`src/fn/sub_081F065C.c`).

* Compiler: `agbcc -O2 -mthumb-interwork`. Leaf functions may end in
  `bx lr` with no push. Functions that call others end in `pop {r0}; bx r0`
  (void) or `pop {r1}; bx r1` (returns a value), so the return type shows
  in the epilogue.
* `movs r0, #0; str r0, [rX]; pop {r1}` at the end: write
  `return *(u32 *)(p + off) = 0;` with a `u32`/`s32` return type.
* Parameter widths matter: `lsls r1, #0x10; lsrs r1, #0x10` at entry means a
  `u16` parameter, and a missing truncation means `u32`/`s32` (a `u8` that
  should have been `u32` won't match).
* Trailing data after a function and labels other code points into are
  handled by the build (since commit "split: keep trailing data..."). If
  linking reports an undefined `_08XXXXXX` / `__addr` symbol, report it as a
  tooling bug in your notes instead of working around it.
* agbcc derives one RAM address from another (`subs r0, #0x2c`) when a
  function uses several nearby constants. Declaring each address as its own
  `extern T gUnk_<addr>;` gives each one a separate literal, as the original
  usually has. Assigning `p = (T *)CONST;` as a separate statement stops the
  offset being folded into the literal.
* Declaring a constant local first (`u32 m = 4;`) moves the constant load
  before the memory operations. A struct copy `*(struct P *)a = *(struct P *)b`
  produces paired `ldr`/`ldr [,#4]`.
* Register-allocation differences can come from declaration order or a
  missing or extra temporary. Try reordering locals, splitting or merging
  expressions, and `u8`/`u16`/`s16` types for loads (`ldrb`/`ldrh`/`ldrsh`).
* Branch order: `if (!x) return A; ...` and `if (x) {...} else return A;`
  produce different layouts. Try both. A single `return` with a result
  variable often matches better.
* `cmp rX, #n; beq; cmp rX, #n; bgt ...` is a switch statement.
* **Never write a raw ROM data address** (`0x08603300`) in C: it matches but breaks the shift test. Declare the gen/data.s label (`extern const u32 gUnk_08603300[];`). build.py rejects raw addresses >= 0x0824DAFC.
* Struct field offsets: define a local struct with observed padding and field
  types. Raw casts can change alias information, pointer reloads and address
  lifetimes; they are not always interchangeable with typed fields. The solo
  family tranche matched 0818D754 and 080F57E8 on their first typed checks.
  See `notes/solo-family_2026-10-09.md` for the layout and its limits.
* Multiplications by constants appear as shift/add sequences, and
  divisions as `bl __divsi3` / `Div` (svc 6).

