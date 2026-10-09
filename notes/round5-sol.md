# Sol production round 5

Baseline `848f2579517dd25267b56953955a2218044eec94`; branch `codex/endurance-r5-sol`.

15 net new exact matches, 2,124 emitted bytes. Five manually solved family seeds (one assisted by the permuter), six manual template/sibling ports, four automatic ports. All new source files were absent at baseline. Ownership and reserved ranges respected. No make/shared-gen regeneration, shared headers/tools/symbol edits, push, or emulator jobs.

The 288-byte projection-and-buffer pair 08199790/081999CC initially differed only in three commutative add operand orders. A 90-second permuter trial found score zero. Its useful change is ordinary C: a named stride, assigned `stride=4` in the first index multiplication and reused for the next two writes. This retained the desired base-plus-offset ordering. The accepted source was manually reconstructed from that change and checked exact. Separating actor/state pointer locals, caching x/z before projection, returning a local from the signed rounding helper, and fusing projected y/z with screen-offset adjustment had already removed earlier allocation/spill differences. Ten distinct manual candidates including final verification; one duplicate compiler diagnostic invocation of the initial invalid candidate is recorded separately.

First-check matches: packed-coordinate angle states 08185560/0818561C (188 each), allocation-mask scanner 0819A07C/0819A1D8 (100 each), timer transition 08171D08/08174078 (84 each), packed-field script 081E51A4/081E5264 (192 each). Reused C templates and manual corrected offsets also yielded 081CA900 (184), 081A3710 (92), 081F21C0 (52), 081B6FFC (60), and 0817A41C (32). Loose immediate translation does not replace multi-instruction constants, e.g. stride 0x258 to 0x234 or allocation size 0xEE4 to 0x1248; manual checks caught these.

Retained candidates:

- `wip/round5/sub_081B9BC0-best64.c`: same-size 64 bytes, only two adjacent parameter-copy instructions exchanged; other instructions match. Its sibling 081C0478 has identical body. A 60-second trial improved score30 to20 but did not match.
- `wip/round5/sub_081FEDA0-best72.c` and `wip/round5/sub_081FB82C-best64.c`: same-size lifecycle routines using SDK CpuFill32 macro, only source-SP move versus control-literal load exchanged. Siblings 0820D258 and 081FBEB8 available. A 60-second permuter trial for FEDA0 saved no candidate; SDK expanded preprocessor output gave base score65.
- `wip/round5/sub_08187230-first912.c`: complete ordinary-C draft of a roughly956-byte distance/state machine, with 912 emitted bytes and extensive allocation/layout differences; later experiments 900/912 bytes are separately snapshotted. No speculative semantic renames.
- Retained 08194858 and 082038A8 received two brief trials each without progress. Older ignored WIP remains intact. 0818E1BC latch helper remains 72 versus68 bytes; related variants available.

Three direct full builds print `build/boktai3.gba: OK`: `build/round5/fullbuild-01.txt`, `fullbuild-02.txt`, and `fullbuild-03.txt`. Every matching TU has its own commit after complete-build validation. Parent handles combined emulator validation. Exact manifest and ordered SHAs are in ignored `build/round5/results.json`.

Logged checks: 57 ({'match': 15, 'mismatch': 39, 'error': 3}); one duplicate diagnostic compiler error gives 58 total compiler invocations. Logged check time 28.64s. Matching stopped at 18.41 elapsed wall-clock minutes; final manifest preparation brought total elapsed time to 20.44 minutes.
