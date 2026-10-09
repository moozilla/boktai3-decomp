# Round 4 — Luna B

Base: `73847dde2ef9ed8d739a9977b99c074966fb920d`; branch `codex/endurance-r4-luna-b`. Range `080A0000 <= addr < 08170000`; root-reserved `08160EA4`, `081616D0`, `08161AF4`, `08161F18` excluded. Start 2026-10-09 02:53:05 UTC.

No exact/loose clone port candidates were accepted in the range. Baseline was 380 clone families / 1251 functions; 12 unmatched members had matched family templates, all outside this range. Interesting unseeded families include `08085D58 / 080A2048 / 080A5B8C / 080ACCF0 / 080E7D08` and `08086B6C / 080A6030 / 080AD1C0`.

`080A2048` is an unseeded clone family representative (51 insns). Retained candidate `wip/sub_080A2048.c` matches through `080A207C`; the target branches at `080A207E` to the compare at `080A2082`, but agbcc branches directly to the final skip at `080A20A8`. Permuter score improved 40 to 0 in 60 seconds, but its candidate still fails `check.py` on that branch target; score normalization ignores this distinction. A prior close candidate's last constant load order also differed before the permuter rewrite.

`080A6030` (77 insns) is an unseeded two-member clone-family representative. Retained `wip/sub_080A6030.c`; the queue-byte consume matches, and a structured draft matched both object flag update paths. The remaining `arg > 0x1A` initialization path differs in register/constant derivation and shifts the exit labels; latest attempts with a function-pointer temporary worsened allocation. Do not count as matched.

Round4 has 0 verified functions / 0 emitted bytes and no function commits. Clone sweeps returned 0 ports. Near-match WIPs are preserved in `wip/`; full-build evidence and attempt timestamps are in ignored `build/round4/luna-b.log`.
