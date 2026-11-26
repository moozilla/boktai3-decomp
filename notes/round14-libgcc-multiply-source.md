# Libgcc integer multiplication: source evidence

Tags: #matching #cleanup #round14 #libraries

`Libgcc_MultiplyInt64` (`0824AC74`, `src/lib/libgcc/libgcc_multiply_int64.c`) comes directly
from `pret/agbcc` commit `da598c1d918402c42c0c0d7128ba14567f3175e9`:

* `libgcc/libgcc2.c`, `__muldi3`, assigns the product of its two low words
  through `__umulsidi3`, then adds the two cross products to the high word.
* `libgcc/longlong.h`, `__umulsidi3`, creates a `DIunion` result in a GNU
  statement expression and invokes `umul_ppmm`.
* `libgcc/longlong.h`, `umul_ppmm`, uses the `do { ... } while (0)` block
  retained in this decompilation. Its four halfword products, carry test,
  high-word assignment and low-word assignment are unchanged.

The source was verified by preprocessing the pinned `libgcc2.c` with
`-undef -nostdinc -I build/tools/agbcc/ginclude -DL_muldi3`. Ignoring
whitespace, the entire pre-change function body is the same 447-token
sequence as the original `__muldi3` body; only the function name differs.
The original license remains in the source file.

The outer statement expression can become an ordinary block that assigns
the completed product to `w.ll`. Stock `old_agbcc -O2`, the existing
library configuration, still produces the exact 112 bytes.

Natural removal of the inner loop was also tried. Keeping its body as an
ordinary block changes 12 bytes; writing the partial product directly into
`w` changes 31 bytes. Both candidates retain the original 112-byte extent.
Those candidates are not landed.

The retained loop therefore has one narrowly justified annotation:

`concession-ok(once-loop): GCC libgcc/longlong.h umul_ppmm expansion, called by libgcc2.c __muldi3`

Its justification is the independently recovered original source. The
byte differences explain why the natural simplification was not landed;
they are not the evidence supporting the annotation. No private macro or
compiler configuration was added.
