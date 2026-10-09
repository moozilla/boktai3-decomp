# Third-party code and data

Everything written for this project is MIT-licensed (see `LICENSE`). Several
inputs come from elsewhere:

| What | Where | License | How it is used |
|---|---|---|---|
| **The game ROM** | © Konami | proprietary | **Not included.** You supply `baserom.gba`. Generated asm and extracted data are derived from it, so they are never committed (`build/` is gitignored). |
| `tests/saves/ShinBok2.sav` | the 2007 fan-translation project ([boktai3trans](https://github.com/moozilla/boktai3trans)) | same authors | late-game battery save used by the emulator tests |
| **pret** projects: [agbcc](https://github.com/pret/agbcc), [pokeemerald](https://github.com/pret/pokeemerald) (MP2K `m4a.c`, SDK headers) | GitHub | agbcc: GPL (GCC-derived). pokeemerald and most pret repos have **no license file**, so as far as we know they are unlicensed. | agbcc is fetched and built by `tools/setup.sh`, not vendored. Library C taken from pokeemerald (Nintendo SDK code reconstructed by pret) is marked with its origin in each file. We follow the common practice of other GBA decomps here; it carries pret's licensing status, not this project's MIT license. |
| `src/lib/libgcc_dp.c`, `src/lib/libgcc_fp.c` | [pret/agbcc libgcc/fp-bit-base.c](https://github.com/pret/agbcc/blob/da598c1d918402c42c0c0d7128ba14567f3175e9/libgcc/fp-bit-base.c) | GPL-2.0-or-later with the original linking exceptions; see the source headers and [license text](licenses/GPL-2.0.txt) | Specialized to the original double/float configurations; function aliases and existing RAM NaN objects preserve the ROM layout. These files are not MIT-licensed. |
| `src/lib/libm_*.c`, `include/libm_compat.h` | [newlib 1.8.2](https://sourceware.org/pub/newlib/newlib-1.8.2.tar.gz), Sun fdlibm sources | Sun's permissive notice, retained in each file | Exact math-library source matches; existing ROM constants are referenced rather than duplicated. See [docs/LIBM.md](docs/LIBM.md). |
| `src/lib/libc_memcpy.c`, `src/lib/libc_memset.c` | [pinned pret/agbcc libc/string](https://github.com/pret/agbcc/tree/da598c1d918402c42c0c0d7128ba14567f3175e9/libc/string) | Cygnus Solutions permissive notice, retained in each file | This software was developed at Cygnus Solutions. Exact matches using old_agbcc; these notices are separate from this project's MIT license. |
| EEPROM units `src/fn/sub_08248634.c`, `sub_082486B4.c`, `sub_08248778.c`, `sub_082488D8.c` | [libgbabackup](https://github.com/laqieer/libgbabackup) / [zeldaret/tmc](https://github.com/zeldaret/tmc) | No root license file in the pinned repositories | Adapted reconstructed SDK C; not covered by this project's MIT license. Pins and behavior evidence: [docs/EEPROM.md](docs/EEPROM.md). |
| `src/lib/librfu_*.c`, `include/rfu_compat.h` | [pinned pret/pokeemerald RFU SDK reconstruction](https://github.com/pret/pokeemerald/tree/731ad5bfd6e6f265508d0efcca0ba42f9dcf5881) | No root license file | Version 1024 source, types and constants; exact Thumb and ARM matches. Retains upstream licensing status, not MIT. Evidence: [docs/RFU.md](docs/RFU.md). |
| [gbadisasm](https://github.com/jiangzhengwenjz/gbadisasm) (pret lineage) | GitHub | no license file | fetched by `tools/setup.sh`; `tools/patches/gbadisasm-no-assert.patch` is our small change to it |
| [armips](https://github.com/Kingcom/armips) | GitHub | MIT | fetched by `tools/setup.sh` |
| [decomp-permuter](https://github.com/simonlindholm/decomp-permuter) | GitHub | MIT | pinned source fetched into ignored `build/tools/`; optional candidate search via `tools/permute.py` |
| [mGBA](https://mgba.io) (`libmgba`) | system package | MPL-2.0 | linked by `tools/emu/harness.c` |
| [Ghidra](https://github.com/NationalSecurityAgency/ghidra) | NSA | Apache-2.0 | optional; scripts in `tools/ghidra/` |
| Function names from raphaelr's `region_select.asm` | [boktai3trans PR #2](https://github.com/moozilla/boktai3trans/pull/2) | — | names/comments credited in `symbols/functions.csv` (`source` column) |

If you hold rights to any of the above and want something changed, open an issue.
