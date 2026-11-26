# Round 14: indexed palette-bank traversal

Tags: phase3, once-loop, palette, traversal, register-allocation

Base: `c5883ff7d72e754337895289628075d887ba37c2`.

The three repeated palette-bank wrappers now use ordinary indexed traversal.
All three destination-increment once-loops and their obsolete layout comments
were removed. Each complete source unit independently matches stock bytes.

| Function | Address | Source commit | Function size | Complete unit checked |
|---|---|---|---:|---:|
| `Vanargand_BlendPaletteBanks` | `081A55D4` | `8ff81912e` | 64 bytes | 64 bytes |
| `RegistrySelection_BlendPaletteBanks` | `081B9BC0` | `de7e3bcc1` | 64 bytes | 3,604 bytes |
| `ScenePaletteFade_BlendPaletteBanks` | `081C0478` | `7a8fec89e` | 64 bytes | 1,748 bytes |

## Source evidence and adopted algorithm

`Palette_InterpolateBanks` in `src/engine/anim/palette_interpolate_banks.c` reads two arrays of sixteen
halfword colors and writes sixteen interpolated colors. Its final two arguments
are the interpolation weight and scale shift. `struct PaletteResource` in
`include/engine/shared/palette_resource.h` independently declares the resource payload as
`u16 banks[16][16]`; `PaletteSetEntry` also stores its color sets as sixteen
halfwords. Thus the wrappers' `0x20` byte stride represents one complete bank.

The accepted source views the existing byte-pointer arguments as pointers to
sixteen-color banks and uses `for (bank = 0; bank < bankCount; bank++)` to pass
the corresponding output, first-source, and second-source bank to the existing
interpolation function. The public prototypes remain unchanged. Vanargand's
previous single-letter arguments now describe the output, sources, weight,
shift, and bank count.

The three wrappers are the same real operation. No shared helper was necessary
to recover it: the ordinary indexed loop matches directly. A second candidate
using byte bases plus `bank * 0x20` also matches all three functions. The typed
views therefore express the established bank layout; their presence is not
needed to manipulate allocation. No increment-order permutations, dummy uses,
new weighting loops, or diagnostic suppressions were used.

## Measured alternatives

All three functions produced the same results for these six ordinary candidates:

| Candidate | Result for each 64-byte function |
|---|---|
| Remove only the increment wrapper | Six differing bytes; same instruction shape |
| Counted `for` loop advancing byte cursors | 50 differing bytes; different loop instructions |
| Index immutable byte bases by bank stride | Exact |
| Counted `for` loop advancing typed bank cursors | 46 differing bytes; different loop instructions |
| Index actual sixteen-color banks | Exact; adopted |
| Counted `for` loop advancing halfword color cursors | 46 differing bytes; same limit as typed cursors |

The old plain-deletion mismatch is an allocation-order effect. In the Vanargand
diagnostic, the output cursor has seven weighted references, a 15-instruction
live range, and priority 9,333; the decrementing count has priority 10,000 and
takes r4 first. The counterfactual `AGBCC_PRI=x:+668` matches. The accepted
indexed traversal changes the actual loop representation and needs no knob.
This diagnostic was used to explain the old source, not as an acceptance check.

## Verification and durable evidence

Stock `tools/check.py` verification used each complete original unit extent
listed above. Focused linter comparisons remove one `once-loop` warning per unit
and add no finding. The unrelated three `const-local` warnings in the registry
unit and the existing address-expression warning in the tether unit remain
unchanged. No shared header changed, so there are no new header dependents to
check. The coordinator owns combined integration and full-ROM verification.

[round14-palette-traversal-candidates_2026-11-16.jsonl](evidence/round14-palette-traversal-candidates_2026-11-16.jsonl)
contains 22 complete source states as independent patches against the base
commit, including the three accepted states. Each patch was reconstructed and
hash-verified; source hypotheses, historical function metrics, accepted commits,
and full stock extents are included. Generated assembly and RTL are excluded.
Complete source files, stock logs, focused linter reports, and portable histories
remain under `build/round14/palette-traversal/`; live graphs are
`build/matchsearch/round14-palette-traversal-*`.
