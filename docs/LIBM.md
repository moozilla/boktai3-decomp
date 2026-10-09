# Math-library source matches

Twenty-two additional native functions, **11,584 compiled bytes**, match the
Sun fdlibm implementation distributed in
[newlib 1.8.2](https://sourceware.org/pub/newlib/newlib-1.8.2.tar.gz) with
`old_agbcc -O2 -fno-builtin`. Each adapted translation unit was linked at its
original address with every call and data reference resolved, then compared
byte-for-byte. These are source matches, not approximate numerical substitutes.

The downloaded archive has SHA-256
`e52667ed78595dd1919f4a303f8b658b1d57b926e085e8021851b279143cf3ef`.
Sources are `newlib/libm/math/` except `s_scalbn.c`, `s_copysign.c` and
`s_matherr.c`, which are in `newlib/libm/common/`. The original Sun permission
notice is preserved in every adapted file and in `include/libm_compat.h`.
These files retain that license rather than becoming project-authored MIT code.

| Start | Source function | Compiled bytes, including alignment |
|---|---|---:|
| `0824ACFC` | `atan` | 884 |
| `0824B070` | `cos` | 176 |
| `0824B120` | `fabs` | 24 |
| `0824B138` | `sin` | 180 |
| `0824B1EC` | `tan` | 112 |
| `0824B25C` | `acos` wrapper | 184 |
| `0824B314` | `asin` wrapper | 184 |
| `0824B3CC` | `fmod` wrapper | 208 |
| `0824B49C` | `__ieee754_acos` | 1,384 |
| `0824BA04` | `__ieee754_asin` | 1,240 |
| `0824BEDC` | `__ieee754_fmod` | 664 |
| `0824C174` | `__ieee754_rem_pio2` | 1,132 |
| `0824C5E0` | `__ieee754_sqrt` | 516 |
| `0824C7E4` | `__kernel_cos` | 508 |
| `0824C9E0` | `__kernel_rem_pio2` | 2,008 |
| `0824D1B8` | `__kernel_sin` | 384 |
| `0824D338` | `__kernel_tan` | 960 |
| `0824D6F8` | `floor` | 396 |
| `0824D884` | `isnan` | 32 |
| `0824D8A4` | `matherr` | 20 |
| `0824D8B8` | `scalbn` | 348 |
| `0824DA14` | `copysign` | 40 |

`symbols/proposed/libm.csv` records names and their exact-source evidence.
Address aliases preserve the existing symbol map. `__errno` at `0824DA3C`
was independently checked against its callers but was already C; it earns
no additional matching credit or function boundary. No denominator changed.

## Reproduction and adaptations

The first pass scanned all 247 built objects in the pinned agbcc libc and
found five signatures: scalbn, copysign, isnan, memcpy and memset. The bundled
libc contains only a subset of the math library. A broader scan of the older
fdlibm sources found the larger kernels above. Relocation-masked signatures
served only as candidates; fully linked equality was required afterward.
The acos/asin wrappers each had two masked hits. Resolving their distinct
core calls and name strings disambiguated them; both complete objects match.

The initial broader source was the Chromium newlib mirror at
`5feee65e182c08a7e89fbffc3223c57e4335420f`. Its `e_asin.c` did not match.
The historical 1.8.2 source has a different bracing scope in the small-input
path, and that version matches exactly. The source revision matters even when
the high-level formula and most generated instructions are the same. This
does not establish that the game used an entirely stock 1.8.2 distribution.

Adaptations specialize the upstream preprocessor branches and expand its
word-access macros. The compatibility header preserves the compiler's
high-word-first double layout and the source's 32-bit integer types. Scalar
compile-time constants become macros. Constant arrays and the `fmod` name
string reference existing generated data labels; no new tables, strings or
global storage are inserted. Every referenced constant block was also compared
against the upstream object's bytes at its resolved ROM address.

Keep arrays declared as arrays. Casting an unsigned-byte label to a double
pointer changed agbcc's base-plus-offset instruction selection and register
allocation, despite equivalent accesses. Declaring `extern const double
gUnk_...[]` reproduced the original array addressing and restored exact matches.
Compiler-generated floating-point calls require linker aliases as well as C
macros; their addresses are independently supported by the prior exact libgcc
units and by the new linked call instructions.

Local audit files are under `build/endurance/root/`: the downloaded archives,
`newlib182-scan.log`, `libm182-adapt.log`, `libm-aliases.json`, adapted WIP,
and linked comparisons in `build/check/`. The final full ROM and combined
shift-test logs are recorded with the integration batch. All generated or
ROM-derived artifacts stay untracked. The mathematical matches do not by
themselves explain where gameplay uses these operations.
