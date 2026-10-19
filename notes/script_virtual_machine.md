# Script virtual machine, merged unit

Range **0821A454–0821BAFC**: the GCL bytecode interpreter proper (operand
decoding, argument/keyword stacks, native-command dispatch, procedure lookup,
the postfix expression evaluator, and the game-state/script-variable regions
it reads and writes). All 89 functions in the range are exact; `src/fn/*.c`
for this range is gone, replaced by `src/script_virtual_machine.c` with shared
types in `include/script_virtual_machine.h`.

## Header

`include/script_virtual_machine.h` adds:

* `struct ScriptVm` for `gUnk_02000610`: `pc`, `result` (last eval/call result,
  read by callers of blocks), `keywordSp`/`stack[24]`/`argSp` — the keyword
  (control) stack and the call-argument stack share the same backing array,
  growing up from opposite ends (`argSp` resets to `stack + 25`).
* `struct ScriptProcedureTable` for `gUnk_02000438` (the loaded script's
  procedure table: entries, count, body base, main block) and
  `u8 *gUnk_02000448[4]` for the loaded script's four resolved section
  pointers (string table, string base, and two more).
* `struct ScriptCommandTable` / `struct ScriptCommandEntry` for the native
  command table linked list at `gUnk_02000458`.
* Reused `struct ScriptUtilityReference` (`script_reference.h`) and
  `struct ScriptUtilityArguments` (`script_utility.h`) — both already existed
  for this exact object, declared by other units that call into this one.
  `script_utility.h` also owns the external prototypes for `Script_ReadValue`
  and `Script_RunBlock`, so this header does not redeclare them.
* `include/engine.h` (included transitively) already declared
  `Script_GetValue`, `Script_SeekToKeyword`, `Script_GetPc`,
  `Script_ParseStringRef`, `Text_LookupString`, `Script_GetKeywordValue`,
  `ScriptProc_Start` and `Script_Call` — reused as-is, not redeclared.
* `include/game_state.h` gained four externs: `gUnk_0200070C`/`08`/`04`/`00`,
  the backup game-state base, the active/backup shared script-variable region
  and the (unbacked) local-variable region, alongside the existing
  `gUnk_02000710`. They're set together by `GameState_SelectRamBase` and used
  by one other, not-yet-merged function (`sub_0822BC6C`) outside this range.

## Local adaptations

* `gUnk_02000710` is `struct GameState *` (from `game_state.h`); this unit's
  functions treat it as a raw byte region (`Memory_Clear`/`Memory_Copy`,
  pointer subtraction, `+0x30`/`+0x744` offsets), so those call sites carry an
  explicit `(u8 *)` cast. Confirmed by a temp test with agbcc that the cast
  adds no instructions — it only changes what the C type checker is told.
* `engine.h`'s declared types are occasionally wrong for *this* implementation
  (established by diffing the merged build against this project's own agbcc,
  not guessed): `Script_SeekToKeyword`'s real parameter is a byte (the
  prologue zero-extends it) despite the `s32` prototype, and likewise
  `Script_GetKeywordValue`'s first parameter. Narrowing the shared prototype
  to `u8` would add truncation instructions at the ~300 other call sites across
  the codebase that pass it a non-constant `int` (verified with a throwaway
  agbcc test), so `engine.h` keeps `s32`; each function instead truncates its
  own parameter into a local `u8` first (`u8 k = keyword;`), which reproduces
  the real prologue without touching the shared prototype. Flagging this
  rather than fixing it silently, per the worker brief.
* Several "builtin command" handlers (`Script_ExecuteIf` and, it turns out,
  two of the newly-named gap functions) are invoked indirectly through
  `Callback_InvokeActor`/a stored function pointer with an argument the
  original C never declared, relying on the register still holding the
  caller's value. Since nothing in this unit calls them by name with a literal
  argument count, they got a real declared parameter instead of the implicit
  passthrough — cleaner and still byte-exact.
* `Script_InitRuntime` is a genuine same-file register-passthrough: it calls
  `ScriptProc_Start(0xA8D9, 0x49BC)` then falls into `Script_LoadScript` with
  no argument, relying on r0 carrying the previous call's return value (visible
  in the ROM: `bl ScriptProc_Start` is immediately followed by
  `bl Script_LoadScript`, no register setup between). Giving `Script_LoadScript`
  a real prototype (needed since its own definition appears earlier in the
  merged file) makes a 0-argument call to it a hard compile error once both
  are in one translation unit (confirmed: GCC/agbcc checks argument count
  against any visible full prototype, even mid-file). The call is instead
  written as `((s32 (*)())Script_LoadScript)();` — a cast to an
  unprototyped function-pointer type, which agbcc compiles to the identical
  bare `bl Script_LoadScript` (confirmed byte-identical) without tripping the
  argument-count check. No register pin, no handwritten instructions.
* `sub_0821AB78` is a byte-for-byte duplicate of `Script_GetValueSafe`
  (0821AB88) kept as a separate ROM function; noted rather than merged away.

## Gap functions found and added

`tools/unit_files.py` found no gaps up front because these three were never
flagged as functions at all — gbadisasm left them as `.byte` blobs with no
inbound `bl` anywhere in `gen/code.s`/`code_sym.s` and no pointer evidence in
`symbols/pointers.txt`. They surfaced only because merging shifted every
later address and the whole-unit check stopped matching; each was found by
bisecting the first wrong `bl` target back to a byte-identical gbadisasm
dump, decoding it by hand (`arm-none-eabi-objdump -b binary -Mforce-thumb`),
and writing C for it. All three are unreferenced by anything found so far
(added to `symbols/progress_extra_functions.csv`, sorted; inventory refreshed
with `tools/progress.py --refresh-inventory`):

* **0821AAF4** (36 bytes): scans operands from a hidden cursor argument until
  the terminating zero type, then clears the VM's PC. `Script_ReadNextKeyword`
  with no keyword match, essentially.
* **0821ABF8** (56 bytes): unlinks a node from the command-table list at
  `gUnk_02000458` by pointer identity — the removal counterpart to
  `Script_RegisterCommandTable`. Returns 0 if found in the middle, -1 if it
  was the head or not found at all (a real quirk of the ROM code, not a
  simplification: removing the head still falls through into the walk, which
  can no longer find the now-detached node).
* **0821AD98** (60 bytes): binary search by key over a `ScriptArchiveEntry`
  table (same shape as `Script_LookupArchive`), but returns the entry pointer
  itself rather than its value field.

None of the three have a confirmed caller yet; whatever calls them (directly
or through a pointer table) hasn't been identified. Left `sub_<ADDR>` names.

## Surprises

* The VM's keyword/control stack and its call-argument stack are the *same*
  backing array (`gUnk_02000610.stack[24]`), growing from opposite ends — not
  two separate stacks. `Script_ResetArgumentStack` sets `argSp = stack + 25`,
  one word past the declared 24-element bound; harmless since it's pure
  pointer arithmetic, not an indexed access, but worth flagging as a hint that
  the real buffer may extend further than any one function's view shows.
* Three agbcc behaviors were confirmed empirically with a scratch `.c` file
  compiled directly against this worktree's pinned `agbcc` rather than assumed:
  a full prototype in scope makes a too-few-arguments call a **hard error**
  (not a warning) even when agbcc still happens to emit the desired bytes
  (and `tools/build.py` treats that as a failed compile regardless); a
  **type-only** mismatch at a call site (same argument count) is just a
  warning; and a **definition** whose type disagrees with an already-visible
  prototype is always a hard "conflicting types" error, for return type,
  signedness or pointer type alike.

## Process

Roughly 90 `tools/check.py` iterations total: most functions needed one pass
(the individually-matching `src/fn/*.c` files already existed; the work was
retyping their ad hoc structs to the shared header and fixing the handful of
prototype collisions above), the three gap functions took 2–4 iterations
each, and the whole-unit check after merging took about 6 bisection rounds
to find the two gaps that weren't visible function-by-function. One real
build pending at batch integration; not run here per the worker brief.
