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

Arrays of structs are replicated per element (`slots[0].x`, `slots[1].x`, ...), and
an unsized scalar array (`u16 g[]`) extends as far as any other evidence reaches.

## How it merges

Every placed field covers a byte range. A field is **opaque** when its own name is
a placeholder: `pad`, `unk`, `unknown`, `field`, `filler`, `reserved`, ... with or
without an offset suffix; a letter plus an offset (`f54`, `a720`); or a byte array
named with one letter (`u8 f[0x868]`). A `u8 g[]` declaration of the whole object is
opaque. Everything else is **named**. Per byte, the tool records which files name it
and which leave it opaque. The report lists:

* **Offset map**: every offset where a field starts or the ROM accesses. Columns:
  names with type and files, opaque members, ROM accesses (width, `s`/`u` for
  `ldrsh`/`ldrh`, `ld`/`st`, count), and C casts. **conflict** marks named scalars
  at one offset with different widths or signedness. **unnamed, used in ROM**
  marks bytes the ROM touches that no file names.
* **Conflicts**: each disagreement, with the ROM's load widths and signs as the
  tie-breaker (`ldrsh` says signed, `ldrh` unsigned; stores say nothing about sign).
* **Opaque in one file, named in another**: the ranges to fill in first, split
  per name.
* **Named by no file**: opaque everywhere (or declared nowhere), with how many of
  those bytes the ROM or a C cast touches.

The summary's main column, *opaque, named elsewhere*, counts bytes that some file
leaves opaque and a different file names: the first candidates for a shared struct.

## ROM evidence

`arm-none-eabi-objdump -d build/boktai3.elf` up to the end of code, scanned function
by function. The scan follows registers through literal-pool loads (`ldr rX, [pc,
#n]`, value read from the ELF), `movs`/`mov` copies, `adds`/`subs` immediates,
`lsls` of constants, and register-plus-constant sums. It records every
`ldr*`/`str*`/`ldm`/`stm` whose base is a known RAM literal L at L + k.
A word loaded from a RAM address and then used as a base gives pointee accesses of
that address. Register state ends at unconditional branches, `bx` and `pop {pc}`,
and `bl` clobbers r0-r3 and ip. Literal pools inside a function are skipped. The
scan is linear and heuristic: at a branch target the state from the fall-through
path is kept.

The report lists each literal-pool base used in the object's extent, with the `#k`
offsets used through it. In agbcc's Thumb output, one literal used with several
offsets means the original code accessed a struct, so the object is one struct. One
literal per field (the literal itself is `g+0x22`, always used at `#0`) means the
original declared separate globals there. The ROM cannot see accesses through
pointers passed as arguments, so pointee evidence is limited to code that loads the
global itself.

## Limits

* Declarations inside function bodies (block-scope `extern`) are not read, and
  structs that agbcc cannot compile (anonymous members) give no offsets.
* Casts are read by regular expressions on preprocessed text. Unusual spellings
  are missed, but the common `*(T *)(g + K)`, `((struct X *)g)->` and `g[K]` forms
  are covered.
* A global is reported by the symbol at its start. A struct that spans later symbols
  (`Other symbols inside this extent`) also appears, with its own declarations,
  under those symbols.
