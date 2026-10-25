# Subsystem unit worker

Turn one subsystem (a contiguous ROM range from `symbols/progress_subsystems.csv`)
into a single translation unit `src/<subsystem>.c` with shared types in
`include/<subsystem>.h`. Matching must stay byte-identical. Read `CLAUDE.md` first.

## Setup

Your worktree has its own build outputs; use `build/venv/bin/python` for tools.
List the files covering your range in ROM order:

```sh
build/venv/bin/python tools/unit_files.py 080036F8 08003B44
```

It prints each file with its functions and flags ROM functions in the range that
have no C at all (gaps).

## End state

1. The header holds the real shared structs, built from field offsets seen across
   functions, with provisional descriptive field names (comment guesses). Reuse
   existing headers in `include/` when the object is the same.
2. Functions use the shared types: no per-function partial structs, no `u8 pad[]`
   views of a struct the header defines, no `(u8 *)p + 0x123` where a field fits.
3. One file, made by `tools/merge_unit.py src/<subsystem>.c FILES... --write`. It
   keeps identical repeated declarations once and reports same-named ones with
   different bodies: unify those first.
4. Every function still matches.

## Unmatched functions in your range

Once the types are in place, try to match each unmatched function in your range:
you now hold the subsystem's context, which is the point of working in ROM order.
Use `tools/asmat.py ADDR 80`, write C in the unit at its ROM position, and check
the unit. Budget roughly 8-10 check iterations per function, more for large ones
that are close. If it matches, great (one commit per match). If not:
* close (`tools/check.py --nonmatching` fuzzy above ~80%): keep it as a
  `#ifdef NONMATCHING` draft with `--record` and a one-line note on what differs;
* otherwise leave `INCLUDE_ASM` and note what blocked it in your report.
Do not add register pins to force a match.

## Engine API

`include/engine.h` declares the shared engine functions and globals (tasks, heap,
script reader, sound, sprites/animation, colliders, text windows). Include it via
your header and don't redeclare those functions locally. If its prototype is wrong
for your callers, say so in your report rather than working around it silently.

## Loop

* `tools/check.py src/fn/sub_X.c` per function before merging, then
  `tools/check.py src/<subsystem>.c` after.
* A field type change that breaks a match usually means the type is wrong (width,
  signedness). If a function truly needs its old shape, keep a minimal local
  adaptation with a comment.
* `make format`, then `tools/build.py` must print `build/boktai3.gba: OK` (first
  run ~4 min), and `tools/progress.py` must not report fewer matched bytes.

## Known pitfalls (from the pilots)

* agbcc aligns nested structs to 4 bytes. A nested struct at an offset that
  isn't 4-aligned shifts the layout: keep those fields flat.
* Return types matter: a function whose `r0` is read by callers may need a
  non-void return even without a `return` statement. Keep what matches.
* `merge_unit.py` cannot merge backslash-continued `#define`s: move them to the
  header first.
* **Gaps**: a range may hold small functions gbadisasm left as data or labels
  (`_08XXXXXX:` in `gen/code_sym.s`). A unit must be contiguous, so decompile
  them (they are usually trivial). Name them `sub_<ADDR>` and register each one
  with `tools/add_function.py ADDR` (then `make`). If a gap cannot be
  matched, keep it as `INCLUDE_ASM` in the unit.
* Unmatched functions inside the range stay as
  `INCLUDE_ASM("asm/nonmatching", name);` (or a `#ifdef NONMATCHING` draft) in
  their ROM position.

## Rules

* No new register pins or `asm("")` barriers; existing ones may stay (run
  `tools/depin.py FILE --write`). No handwritten instructions.
* Don't edit `symbols/functions.csv`, `gen/`, or tools. Don't rename functions.
* Commits: type unification first, merge last; one subsystem per merge commit.
  End messages with your model's `Co-Authored-By` line. Don't push.

## Report

Short: build result, functions newly matched (and drafts with fuzzy %), header structs in brief, local adaptations and why, gap
functions added, surprises, rough number of check iterations.
