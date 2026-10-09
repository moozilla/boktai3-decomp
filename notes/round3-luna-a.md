# Luna A round 3 notes

Matched six fresh functions in 08040000–0809FFFF, producing 788 bytes. All six are ordinary C; no manual asm or raw ROM addresses. The full build passed before each commit group, and the newgame_intro.txt data-shift test passed for screenshots 00–15.

Useful patterns:

- sub_08044DD0 is a four-row byte conversion loop. Keeping the destination base separate and computing each row as base + (row << 8) recovered the stack-spilled base and row counter. Separate *out = table[j]; out++; j++; statements preserved the original output-pointer-before-index update order.
- sub_08044CF4 / sub_08044C80 are paired bitstream read/write helpers. Signedness controls asrs versus lsrs; inline range expressions helped recover the target register use. Pointer expression operand order mattered for the final address add.
- For repetitive object initializers such as sub_0804AA08, a short-lived pointer to the first field plus local zero/two values caused agbcc to preload values before the first store and reuse the address offset as in the ROM.
- sub_08051D28 matched with padded local structs and byte fields at their actual offsets. A volatile or explicit-register workaround was unnecessary.

Round execution evidence and per-target attempt counts are in ignored build/round3/luna-a-log.md.
