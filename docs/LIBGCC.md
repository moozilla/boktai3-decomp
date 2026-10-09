# Floating-point runtime identification

The contiguous double- and single-precision runtime units match
[`libgcc/fp-bit-base.c` in pret/agbcc at da598c1d](https://github.com/pret/agbcc/blob/da598c1d918402c42c0c0d7128ba14567f3175e9/libgcc/fp-bit-base.c).
The pinned compiler's own libgcc Makefile uses **old_agbcc -O2**, without
`-mthumb-interwork`. This is a demonstrated library-specific compiler choice;
it does not change the evidence for regular agbcc in the game code.

| Unit | Original range, end exclusive | Functions | Bytes |
|---|---|---:|---:|
| Double precision | `08249558–0824A2F4` | 20 | 3,484 |
| Single precision | `0824A2F4–0824AC40` | 20 | 2,380 |

`tools/sigmatch.py` first found 14 unique relocation-masked functions in each
object. The six comparator routines in each object are similar enough to be
ambiguous in that scan. Linking the **entire** unit at its ROM address and
resolving all calls and NaN references produced an exact byte comparison,
including all comparators and literal pools. Masked signatures alone were not
used as final proof. The two units preserve the library's original source order.

The source is specialized using the upstream float/double macros and retains
its FSF copyright, GPL terms and linking exceptions. The generated preprocessor
expansion comes from the upstream C, not from ROM disassembly. `__fpadd_parts_dp`
and `__fpadd_parts_fp` disambiguate upstream's two file-local `_fpadd_parts`
helpers; all other aliases retain upstream names. The helpers are made global
so the build accounts for their original function boundaries. Names and evidence
are recorded in `symbols/proposed/libgcc.csv`; there are no inferred gameplay
names in this change.

The original zero-initialized NaN objects already occupy IWRAM at `030035C8`
(double) and `030035E0` (float), confirmed by each unit's three absolute
relocations. The C references those existing objects rather than allocating
new data. Compiler-emitted helpers are resolved through `symbols/ram.ld`:

| Helper | ROM address | Fully linked object bytes |
|---|---|---:|
| `__lshrdi3` | `0824AC40` | 52 |
| `__muldi3` | `0824AC74` | 112 |
| `__negdi2` | `0824ACE4` | 24 |

Each helper object also matches exactly, including alignment padding. The
aliases do not count those three assembly functions as decompiled C.

## Toolchain implication

A source line `// COMPILER: old_agbcc` selects the sibling compiler executable;
unspecified files use regular agbcc as before. `// CFLAGS: -O2` selects the
library flags. The build cache includes the compiler's path and content hash,
and permutation searches use the selected compiler too. Unknown selector names
fail rather than silently choosing a compiler. An environment-level `AGBCC`
override also invalidates cached objects.

The double-unpack function was a useful control: its regular-agbcc object had
four register-instruction differences, while old_agbcc produced the exact
216-byte body. This is why compiler experiments must force a rebuild: the old
cache key did not include `AGBCC`, so switching that environment variable could
otherwise misleadingly reuse the previous result.

Local audit artifacts are under `build/endurance/root/`: `dp-signatures.csv`,
`fp-signatures.csv`, the isolated linked-object comparisons under `build/check/`,
and the batch full-ROM and shift-test logs. ROM-derived binaries and assembly
remain ignored. The full build and shiftability regression are required before
integration.

Eighteen of the 40 function starts were previously hidden inside generated
binary blobs. They are now explicit reviewed progress boundaries (11,046 total
functions; the code-byte denominator is unchanged). The assembly splitter must
consume trailing blobs whose starting address is a function explicitly defined
by the C unit. Otherwise it appends the same hidden code again and shifts every
later routine. A synthetic regression checks that unrelated trailing data is
still retained.
