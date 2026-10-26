# RAM object layouts merged across files

Tags: #tooling #matching

Each draft declares the RAM objects it touches with its own struct: the fields it
understood get names, the rest becomes `u8 unk20[0x38]`. The same object therefore
has many partial declarations. `gUnk_030042A0` has 15 (`struct Vec`, `struct Camera`,
`struct CameraState`, `s16[]`, an untagged struct, ...). The pointee of
`gUnk_02000710` has 48, counting casts like `(struct BarrierGame *)gUnk_02000710`.
Bytes one file pads are often named in another. `tools/layoutmerge.py` connects
these declarations. Use it before inventing names for padding, when writing a shared
header (phase 4), or when two drafts disagree about a field.

```bash
python3 tools/layoutmerge.py --summary
```

```bash
python3 tools/layoutmerge.py gUnk_030042A0
```

```bash
python3 tools/layoutmerge.py --range 03004280-0300523F --summary
```

A cold run over all of `src/` and `include/` takes about 90 seconds on 8 cores, a warm one about 55.
Warm runs read each file's scan from the cache and only preprocess, unless the
tool itself changed. Output goes to `build/layouts/` (ROM-derived, never committed):

* `<global>.md`: one report per RAM global. "The object" is the global itself;
  "the pointee" is `*global` for globals declared as pointers.
* `SUMMARY.md`: all globals ranked by opaque bytes that another file names.
  `--summary` prints the top rows.
* `layouts.json`: everything behind the reports (declarations, placed fields,
  C accesses, ROM accesses) for scripts and external research packages.
* `cache/`: per-file scans, keyed by preprocessed text and the tool's own hash.

## What it collects

For every `src/**/*.c` file and every `include/*.h` (each through a stub that
includes `global.h` and the header), preprocessed with `-DNONMATCHING` so drafts count:

* **Declarations** of RAM globals (any symbol in `build/boktai3.elf` in EWRAM or
  IWRAM; names that share an address, such as `gUnknown_02000580`, merge into the
  `gUnk_` name). `struct X g`, `struct X g[N]` (N copies), `struct X *g` and
  `struct X *g[N]` (pointee), scalars and scalar arrays.
* **Casts to a struct**: `(struct X *)g` adds X as a pointee type, and
  `(struct X *)&g`, or the cast of an array `g`, adds it as the object's type.
* **Typed accesses** without a struct: `*(u16 *)(g + 0x30)` through a byte pointer,
  and `g[3]` with a constant index (offset = index × element size). They appear in
  the "C casts" column.

## How layouts are computed

The tool never computes offsets itself. pycparser (from `build/venv`) only lists member
paths: nested structs are expanded, and arrays of structs through element `[0]`.
A probe compiled by the project's agbcc (`-O2 -mthumb-interwork`) supplies every
number. It is the file's own declarations, with function bodies and initializers
removed, plus

```c
const unsigned __lm_probe[] = {
    sizeof(struct Camera),
    (unsigned)&((struct Camera *)0)->saved.x,
    sizeof(((struct Camera *)0)->saved.x),
    ...
};
```

The tool reads the values from agbcc's assembly output. Where agbcc rejects an entry,
it writes `.space 4` and an error naming the line, so one bad entry costs only that
value. Bitfields cannot take `&`, so each one gets a designated initializer that sets
it to all ones (`const struct T __lm_bf0 = { .a = { [1] = { .b = -1 } } };`); the set
bits in the emitted bytes give the bit position and width. agbcc rounds struct sizes
to 4 (`struct { s16 x, y, z; }` is 8 bytes) and plain `char` is unsigned; both
come from the compiler. Untagged structs are spelled `__typeof__(g)` in the probe.

Arrays of structs are replicated per element (`slots[0].x`, `slots[1].x`, ...). An
unsized array (`u16 g[]`, `struct Bg g[]`) extends as far as other evidence reaches,
and an unsized struct array repeats its fields up to that point.

## Object identity

Objects are keyed by address, never by struct tag: `struct Camera` for the inline
object at `030042A0` and `struct Camera` for the pointee of `03005420` are different
objects in different reports. Struct definitions are keyed by file, line and tag, so
two files' `struct S` never merge by name. Names are only reconciled through offsets.
`ForgingBookCameraPosition.y` sits at +4, where fuller views have `z`, and the report
shows both names at +4.

An object's **extent** is the largest of:

- its declared sizes;
- its fields and C accesses;
- the typed ROM accesses through its own literal;
- a bulk transfer that starts at its address (up to 0x1000 bytes). An example is the
  512-byte `CpuFastSet`/DMA of a palette buffer declared only as `struct PaletteBank16 []`.

Bulk traffic elsewhere doesn't count, because one clear often covers many objects.

**Interior symbols fold into their container.** A symbol inside another object's
extent contributes its declarations to that object at its offset, named
`gUnk_X.field` and marked `via gUnk_X +off`. Its own report says which object it is
inside. Examples are the palette labels `03005160`..`030051A0` inside the 512-byte
buffer at `03004FC0`.

An unsized array of S-byte structs grows through **element aliases**. An element
alias is a symbol at `g + j·S` declared with an S-byte struct and never as an array of
structs. For example, `03004CC0` is `&gUnk_03004C30[3]`, so the background records
reach 4 × 0x30. A symbol declared as an array of its own (`03004FC0` after
`03004DC0`) stops the growth.

## How it merges

Every placed field covers a byte range. A field is **opaque** when its own name is a
placeholder:

- `pad`, `unk`, `unknown`, `field`, `filler`, `reserved`, ... with or without an
  offset suffix;
- a letter plus an offset (`f54`, `a720`);
- a byte array named with one letter (`u8 f[0x868]`);
- a one-letter name in a struct that names its members `a`, `b`, `c`, ....

A whole-object array view (`u8 g[]`, `s16 g[]`) names no field either. Everything else
is **named**. Per byte, the tool records which files name it and which leave it
opaque. The report lists:

* **Offset map**: every offset where a field starts or a typed access lands. Columns:
  names with type and files, opaque members, typed ROM accesses (width, `s`/`u` for
  `ldrsh`/`ldrh`, `ld`/`st`, count), and C casts. **conflict** marks named scalars at
  one offset with different widths or signedness. **unnamed, typed access** marks
  bytes that code reads or writes with a width but no file names.
* **Conflicts**: each disagreement, with the ROM's load widths and signs as the
  tie-breaker (`ldrsh` says signed, `ldrh` unsigned; stores say nothing about sign).
* **Opaque in one file, named in another**: the ranges to fill in first, split per
  name.
* **What the evidence says about each byte**: one class per range (below).
* **Consumers**: every function whose code touches the object, with counts per kind
  of access and the C file that holds it.

### Observation classes

A literal scan never shows that a byte is unused. Each byte therefore gets the
strongest observation the evidence supports:

1. **typed, named**: a ROM load or store, or a C cast, of a definite width, and some
   file names the field.
2. **typed, meaning unresolved**: offset and width are known, but no file names it.
   Camera +C and +10 are both stores at init.
3. **bulk only**: covered only by `ldm`/`stm`, a `CpuSet`/`CpuFastSet`/`memcpy`/`memset`/
   `Memory_Copy`/`Memory_Clear` call with a constant length, a DMA (SAD/DAD/CNT
   register writes), or a single access that spans two declared fields. Camera +6 is
   covered by word copies of the position.
4. **address escape only**: the address reaches a call or is stored, and the
   consumer still needs reading.
5. **unobserved**: nothing in this scan. This is not a claim that the byte is unused.

The summary counts the bytes in classes 2, 3+4 and 5 per object. Its main column,
*opaque, named elsewhere*, counts bytes that some file leaves opaque and a different
file names: the first candidates for a shared struct.

### Storage map vs. a header that compiles

A correct storage map doesn't mean every use can be spelled through it. For example,
in `Bg_UpdateScrollShadow` the rewrite of `*(u16 *)((u8 *)src + 0xb0)` to
`src[3].scrollX` is 8 bytes shorter than the ROM, because the compiler then shares
the record base. Interior symbols may need to stay as their own symbols for the same
reason. A merged declaration is a proposal: compile every function in its
**Consumers** list with `tools/check.py` and require byte-identical output before it
replaces the per-file declarations.

## ROM evidence

`arm-none-eabi-objdump -d build/boktai3.elf` covers everything up to the end of code,
Thumb and the ARM IWRAM routines alike, and the scan reads it function by function.
Functions are the symbols objdump prints from the ELF symbol table, so `sub_`
functions own their own accesses. Nothing is attributed through
`symbols/functions.csv`, which lists only named functions.

The scan follows registers through:

- literal-pool loads (`ldr rX, [pc, #n]`, with the value read from the ELF);
- `movs`/`mov` copies;
- `adds`/`subs` immediates;
- `lsls` of constants;
- register-plus-constant sums.

It records every `ldr*`/`str*` whose base is a known RAM literal L as a typed access
at L + k. `ldm`/`stm` and copy calls are recorded as bulk, and addresses passed in
r0-r3 to other calls, or stored, as escapes. A register that was already used as a
load/store base before the call counts as a pointer the code was using, not as an
argument. A word loaded from a RAM address and then used as a base gives pointee
accesses of that address.

Register state ends at unconditional branches, `bx` and `pop {pc}`, and `bl` clobbers
r0-r3 and ip. ARM conditional instructions forget their destination. Literal pools
inside a function are skipped. The scan is linear and heuristic: at a branch target
the state from the fall-through path is kept.

The report lists each literal-pool base used in the object's extent, with the `#k`
offsets used through it. In agbcc's Thumb output, one literal used with several
offsets means the original code accessed a struct. One literal per field (the
literal itself is `g+0x22`, always used at `#0`) means the original declared separate
globals there. Adjacency is not membership. The ROM's `ObjGfx_ResetAllocationCounters`
compiles exactly only with separate globals at `030042D8`..`030042F4`. The ROM cannot
see accesses through pointers passed as arguments, so pointee evidence is limited to
code that loads the global itself.

## Checked against a manual audit

An external audit of the renderer block (`03004280`-`0300523F`) reviewed the ROM by
hand, and these reports agree with it:

- **`030042A0`**: the 20-byte camera prefix with position, screen offset and mode
  named, +6 bulk only, +C/+10 typed without a name, and +E unobserved.
- **`03004C30`**: four 0x30-byte background records with `03004CC0` as record 3.
- **`03004DC0` and `03004FC0`**: 512-byte palette buffers with the interior labels
  folded in, and `030051F0` left as a separate table.
- **`030042C0`/`C8`/`D0`**: each its own literal, so separate list heads, not one struct.

## Limits

* Declarations inside function bodies (block-scope `extern`) are not read, and
  structs that agbcc cannot compile (anonymous members) give no offsets.
* Casts are read by regular expressions on preprocessed text. Unusual spellings
  are missed, but the common `*(T *)(g + K)`, `((struct X *)g)->` and `g[K]` forms
  are covered.
* Index arithmetic (`base + 32*i + 2*j`) is not summarized into ranges; computed
  offsets are dropped, so an array's later elements are often "unobserved".
* The pointee of a pointer has no address, so nothing folds into it, and what it
  points to (an array of banks, a resource header) comes only from declarations.
