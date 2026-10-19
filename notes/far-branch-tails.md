# Far-branch tails: big functions that were split in pieces

Tags: #matching #tooling (cloud task 08132748, 2026-10-19)

**Breakthrough.** Many of the largest "unmatchable" functions were never whole in the build. agbcc reaches distant code inside a very large function with a far branch: `bl _label` used as a jump, because Thumb `b` only reaches ±2 KB. gbadisasm takes each such `bl` target for a new function `sub_X`. That cuts the owner off in the middle. Its last switch cases, its epilogue and sometimes its pool sit in "functions" that:

* have no `push`, and read the owner's stack frame (`ldr rX, [sp, #0x8c]`);
* leave through the owner's epilogue (`add sp, #N; pop {r3, r4, r5}; ...`) or branch back into it;
* are called only by one or two `bl` instructions from inside the owner.

C for the owner can't match while the split stands: the INCLUDE_ASM boundary is wrong, and the tail looks like a function nobody could write.

**Fix.** Put the tail address in `symbols/function_nonentries.csv`. The split then keeps it inside the owner, so `build/asm/nonmatching/<owner>.s` is the whole function. If a unit `src/*.c` lists `INCLUDE_ASM(..., sub_TAIL)`, delete that line. Run a full build afterwards.

**Find more.** `tools/tailscan.py` lists candidates that aren't recorded yet (no push, no `.4byte` pointer, every caller between the nearest preceding pushing function and the candidate). Review each one first. After this change only two remain:
* `080BEEE8`: recorded in PR #275.
* `0822F5E6`: a real `bx r3` trampoline after `Save_ReadBlockWithRetries`. Leave it.

**Recorded here (35 tails, 17 owners):**
`080898FC`, `080AF2BC`, `080BFCC4`, `080C12FC`, `080C8774`, `080C9C04`, `080CBF24`, `080CE188`, `080D49DC`, `080D812C` (3 tails), `080E0750`, `080E25FC` (2), `080E3874`, `080EE1B0`, `08123194` (2), `08132D2C` (4), `08134398` (4), `PlayerAction_RefreshFlagState` (`0813F702`), `0817C3F4` (3), `081ABA64` (2), `081AE0C0` (2). PR #275 adds `080BD1F0`.

**When matching an owner:** write the whole function, the tail's code included. The far branches come back by themselves. agbcc emits `bl` for any branch over about 2 KB, so the C needs no special form. The worklist's instruction counts for these owners were too small. For example, `08132D2C` is about 2400 instructions with its tails, not 2149.

**Interior data blobs.** An owner can also contain data gbadisasm left as `.incbin` with the owner's
own code after it (08123194 has a 0xdc-byte jump table at 08123FC0). `tools/split.py` used to treat the
first blob in a C function's block as trailing data and keep everything after it as asm, so the
tails were emitted twice ("layout shifted: sub_08124240"). It now consumes a blob that has
instructions after it in the same block.
