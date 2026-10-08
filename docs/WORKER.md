# Worker brief: phase 1 function matching

You are one of several parallel workers. Each worker has its **own git
worktree** and an **assigned address range**, so no two workers touch the same
function or file. Phase 1 is about matched functions. Naming, headers and
merging into real translation units are cleanup for later; don't spend effort
on them.

## Setup (already done for you)

Your worktree is a full checkout with `baserom.gba`, `gen/` (generated
asm, shared, **read-only**) and `build/tools` (agbcc) symlinked in. Run
everything from the worktree root.

## Loop

1. Pick the next function in your range, easiest first:
   `python3 tools/worklist.py --range START END --max-insns 40 | head`
   Skip anything marked `JUMPTABLE` until the easy ones are done.
2. Read it: `python3 tools/asmat.py ADDR 80` (asm) and
   `python3 tools/ghidra_c.py ADDR` (Ghidra pseudo-C, often wrong in detail
   but useful for structure).
3. Write `src/fn/sub_XXXXXXXX.c`, one function per file:
   ```c
   #include "global.h"

   extern u8 *gUnk_02000710;          // RAM: gUnk_<address>, the linker defines it
   extern const u8 gUnk_0860B810[];   // ROM data: use the label that exists in gen/data.s
   void sub_08120DAC(u8 *);           // callees: declare locally, keep sub_ names

   s32 sub_08033568(void) { ... }
   ```
   * Declarations go **in your file**. Don't edit shared headers except to add
     a type that's obviously missing from `include/global.h`, and avoid even that.
   * **No data definitions** (no initialized globals, `static const` tables or
     string literals). Reference existing ROM labels instead.
   * Keep the `sub_XXXXXXXX` name. If you're confident what a function does,
     add a line to `symbols/proposed/<your-worker-name>.csv`:
     `addr,name,confidence,evidence` (e.g. `08033568,Foo_SetFlag,medium,"sets flag 0x17D on gUnk_020000E0 object"`).
4. Inner loop: `python3 tools/check.py sub_XXXXXXXX` compiles just your file,
   links it at its real address and prints original vs. yours side by side,
   with `!` on differing lines (~0.5 s; exit 0 = MATCH). Literal pools show up
   as junk instructions; that's fine. When it says MATCH, run
   `python3 tools/build.py` once (must print `build/boktai3.gba: OK`) before
   committing: it also checks the whole-ROM layout.
5. Matched: `git add src/fn/sub_XXXXXXXX.c symbols/proposed && git commit -m "match sub_XXXXXXXX"`.
   Not matched after ~10 check attempts: delete the file, add a line to
   `notes/<your-worker-name>.md` with the address and what was off (register
   swap, branch order, ...), and move on. Never commit a non-matching file.

**Scratch files:** never use fixed paths under `/tmp` (other workers share it). Put helper scripts in your worktree under `build/` (gitignored) or a directory named after your worker.

## agbcc matching tips

* **Use real struct types, not `*(u16 *)(p + 0x20)` casts**, whenever the
  asm indexes arrays inside an object. Leaf functions that start with
  `mov ip, r0` (369 of them) come from struct/array-of-struct accesses like
  `p->elems[i].flags &= ~1;`. With pointer casts agbcc pushes r4 instead. See
  `src/fn/sub_0810C1E8.c`. Structs also fixed ordering problems elsewhere
  (`sub_081FC61C`).
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
* Register-allocation differences usually come from declaration order or a
  missing or extra temporary. Try reordering locals, splitting or merging
  expressions, and `u8`/`u16`/`s16` types for loads (`ldrb`/`ldrh`/`ldrsh`).
* Branch order: `if (!x) return A; ...` and `if (x) {...} else return A;`
  produce different layouts. Try both. A single `return` with a result
  variable often matches better.
* `cmp rX, #n; beq; cmp rX, #n; bgt ...` is a switch statement.
* Struct field offsets: define a local struct with `u8 filler[N]` padding,
  or use `*(u16 *)(p + 0x20)` casts. Both compile the same.
* Multiplications by constants appear as shift/add sequences, and
  divisions as `bl __divsi3` / `Div` (svc 6).

## Finishing

Stop when your assigned budget is reached or your range has no easy
functions left. Make sure `python3 tools/build.py` prints OK on your final
commit, then report: functions matched (count + list), functions skipped and
why, and any naming insights.
