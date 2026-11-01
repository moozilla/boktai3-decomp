# SceneActorReaction_ApplyHitResponse

Guarded C at 0807D2E0, 91.3% stock fuzzy against a 2,132-byte target. The incoming draft was 57.7%. The current stock compile is 2,136 bytes. It has 82 differing ROM instructions and 83 differing draft instructions in scope's alignment. Differential execution agreed for 128 deterministic inputs reaching 93% of the ROM's basic blocks.

Modes 0 and 1 reset the victim's state and reposition it. In mode 2, both damage paths write the collider's halfword before the actor's response field. The partial-damage path rereads the stored cost, narrows the remaining health to a signed halfword, then clamps it to zero. The volatile BIOS fill source lives at sp+0x10 in both display paths.

The six defeat sparks share a stored random-table pointer and actor position. Assembling the table address from its halfword offset, page and memory region preserves the spill at sp+0x60. The view serves register allocation. The full-width signed flag field also preserves the two zero temporaries after the initial motion-flag clear.

The open allocation starts at 0807D42E. The ROM saves the motion-field address in sl before dispatching the mode and keeps the byte and word zeros in r6 and r5. A separate source reproduces both zeros but still lacks that saved pointer. The mode-0 status-field address and the order of scratch reloads account for much of the remaining difference.
