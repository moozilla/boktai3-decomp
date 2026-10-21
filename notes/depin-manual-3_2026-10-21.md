# depin-manual-3 worklog (2026-10-21)

Worker branch cloud/depin-manual-3. List: wip/depin_list3.tsv (50 functions).
Method: `depin.py --write` over all 50 files, then stripped copies in wip/depin3/ run through `tools/permute.py` (100 s, -j 2), plus hand variants checked with `tools/check.py`. Permuter output is ported by hand and re-checked (score 0 ignores branch offsets, and several score-0 results rely on dummy statements such as `actor++; actor--;` or `s->q = s->q;`, which I did not commit).

## Clean (no pins, no asm)

* Actor_InitLaunchVelocity (080DDD28), SceneActor_ConfigureSecondaryProfileEntry (080DE92C): depin.py alone.
* Nidhoggr_ChangeCompletedPartState (08190C98): `asm("" ::: "r1")` was only stopping 0x71c2 from being derived from 0x7210; two early returns (`if (!*ready) return 0; ... if (*current == state) return 0;`) instead of nested ifs.
* ScreenTransition_FadeBlendOut (081D71CC): the zero store went through an asm operand; now `do { ++p->state; } while (0);` followed by a plain `gUnk_030051D4 = 0;` (the loop wrapper makes the constant materialise after the halfword store).
* ResultMenu_CreateGraphics (0801BCA8): CpuSet through `static inline void CpuSet_2(u32 control, void *dest, const void *src)` (control first); plain locals for src/dst and for the two byte pointers (r8/sb pins gone).
* Vanargand_WaitForActionRelease (081ACAC4): `(*(&p))->pending = 0;` and an extra pointer temp `new_var = &p->delayF20; field = new_var;` replace the r1 pin on the delay value.
* SceneActorBehavior_RequestDescriptorTurn (0807864C): reuse `targetQuadrant` for the shifted value (`targetQuadrant = shifted >> 24`), no separate `direction` variable; three pins/asm gone.
* Axe_TickAuxiliarySpriteTimers (080FBEDC): NEW. The `asm volatile("" : "+r"(one))` only stopped loop.c from hoisting the constant 1 out of the loop. Setting the constant twice (`u32 one = 1;` before the loop, `one = 1;` again inside) makes it non-movable (the pseudo has two sets), so `movs r1,#1` stays in the loop and the outer set is dead.
* Wireless_TryStartParentNiSend (08035F5C): `int check = 1;` (literal, not a copy of `one`), `do { check &= flags; } while (0);`, `1 << *slot`; the volatile pointer cast in the do-while condition also became unnecessary.

## Fewer hints

* Axe_CopyAuxiliaryVector (080FC894): 2 -> 1 (depin.py).
* StatusPanel_DrawDecimalValue (08169A9C): 3 -> 2 (depin.py).
