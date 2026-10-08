# CgbSound (0x08230AC8) - not matched

Best attempt: `notes/cgb_attempt.c` (drop-in replacement for the INCLUDE_ASM). Based on
pokeemerald/pokeruby with fireemblem8u (FireEmblemUniverse/fireemblem8u src/m4a.c) naming/structure
hints. pokeruby, SAT-R/sa1, sa2 and fe8 versions all lack the NR52 check and the stack byte, so none
matches directly.

Already matching: prologue, frame size 0x24, stack layout ([sp]=ch byte, 4=prevC15, 8=soundInfo,
0xc..0x18 nrx0/2/3/4, 0x1c=&ch byte), overall control flow and instruction count, the rsbs for n4=0x80,
n4 frequency high-byte expression (0x3F00 mask), NR52 channel-on test, nrx1 = nrx0 + k derivation.
Key finds: `chIdx = N` must be assigned AFTER nrx4ptr in each switch case (205 -> 131 diff score);
decl order ch, channels, evAdd, prevC15, soundInfo, nrx0..4, chBit, chBitp, chIdx.

Remaining diffs (all register allocation / CSE, ~130 differing lines):
- Original keeps nrx1ptr in r3 and the 0x40 constant in r7, status in r2; ours uses r7/r2/r1 differently,
  so several `ldrb r2`/`r1` and literal-pool offsets shift.
- Original derives nrx4ptr = nrx2ptr + 2/4 (`adds r0,#k`); ours loads a literal (pokeemerald verbatim does
  not derive it either). Permuting the nrx assignment order or chIdx position did not help.
- Original has dead `movs r2,#0` at three spots (before `envelopeVolume=0` and the decay-start/stop paths).
- Original n4=-128 path has its own `strb r3,[r4,#0x1a]` (ours merges with the shared store).
- Original copies REG_SOUNDBIAS_H into a second reg (`adds r1,r0,#0; cmp r1,#0x3f`); ours compares r0.
Tried without effect: volatile prevC15, mask removal, chBitp init position, n4 constant forms,
random decl-order search, REG_SOUNDBIAS_H variable types.

## Permuter (session 2)

`tools/permute.py` on the best attempt above: score 2945 -> 1480 after ~50 min
on 2 cores (0 = match). The best candidate (preprocessed, CgbSound only) is
`notes/cgb_permuted_1480.c`. Resume with
`python3 tools/permute.py build/psrc/m4a_cgb.c CgbSound` after replacing the
function body with that candidate, then run decomp-permuter longer.
