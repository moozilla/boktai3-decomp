# Third-party code and data

Everything written for this project is MIT-licensed (see `LICENSE`). Several
inputs come from elsewhere:

| What | Where | License | How it is used |
|---|---|---|---|
| **The game ROM** | © Konami | proprietary | **Not included.** You supply `baserom.gba`. Generated asm and extracted data are derived from it, so they are never committed (`build/` is gitignored). |
| `tests/saves/ShinBok2.sav` | the 2007 fan-translation project ([boktai3trans](https://github.com/moozilla/boktai3trans)) | same authors | late-game battery save used by the emulator tests |
| **pret** projects: [agbcc](https://github.com/pret/agbcc), [pokeemerald](https://github.com/pret/pokeemerald) (MP2K `m4a.c`, SDK headers) | GitHub | agbcc: GPL (GCC-derived). pokeemerald and most pret repos have **no license file**, so as far as we know they are unlicensed. | agbcc is fetched and built by `tools/setup.sh`, not vendored. Library C taken from pokeemerald (Nintendo SDK code reconstructed by pret) is marked with its origin in each file. We follow the common practice of other GBA decomps here; it carries pret's licensing status, not this project's MIT license. |
| [gbadisasm](https://github.com/jiangzhengwenjz/gbadisasm) (pret lineage) | GitHub | no license file | fetched by `tools/setup.sh`; `tools/patches/gbadisasm-no-assert.patch` is our small change to it |
| [armips](https://github.com/Kingcom/armips) | GitHub | MIT | fetched by `tools/setup.sh` |
| [decomp-permuter](https://github.com/simonlindholm/decomp-permuter) | GitHub | MIT | pinned source fetched into ignored `build/tools/`; optional candidate search via `tools/permute.py` |
| [mGBA](https://mgba.io) (`libmgba`) | system package | MPL-2.0 | linked by `tools/emu/harness.c` |
| [Ghidra](https://github.com/NationalSecurityAgency/ghidra) | NSA | Apache-2.0 | optional; scripts in `tools/ghidra/` |
| Function names from raphaelr's `region_select.asm` | [boktai3trans PR #2](https://github.com/moozilla/boktai3trans/pull/2) | — | names/comments credited in `symbols/functions.csv` (`source` column) |

If you hold rights to any of the above and want something changed, open an issue.
