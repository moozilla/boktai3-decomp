#include "global.h"
extern u16 gUnk_02000544;
extern u16 gUnk_02000500;
extern u16 gUnk_02000520;
extern u16 gUnk_02000534;
extern u16 gUnk_02000590;
extern u16 gUnk_02000570;
extern u16 gUnk_02000528;
extern u16 gUnk_02000518;
extern u32 gUnk_02000560[4];
extern u32 gUnk_02000580[4];
extern u16 gUnk_02000508[4];
extern u16 gUnk_02000548[2];
extern u16 gUnk_0200052C, gUnk_02000550, gUnk_0200053C, gUnk_02000554;
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: hoisted global pointers and loop register allocation.
// confidence=low: clears the link-mode shared arrays and counters in the movement-state tail.
#define Player_ResetLinkState sub_08158C14
void Player_ResetLinkState(void)
{
    s32 i;
    u32 *first;
    u32 *second;
    u16 *half;
    s32 pair;
    typeof(pair) t1;
    *((typeof(gUnk_02000544) *)0x02000544) = 0;
    first = gUnk_02000580;
    second = gUnk_02000560;
    half = gUnk_02000508;
    for (i = 0; i < 4; i++) {
        *(first++) = 0;
        *half = 0;
        *(second++) = 0;
        ++half;
    }

    gUnk_02000500 = 0;
    gUnk_02000520 = 0;
    gUnk_02000534 = 0;
    gUnk_02000590 = 0;
    gUnk_02000570 = 0;
    gUnk_02000528 = 0;
    gUnk_02000518 = 0;
    pair = (t1 = ((s32)gUnk_02000548) + 2);
    do {
        *((u16 *)t1) = 0;
        pair -= 2;
    } while (t1 >= ((s32)gUnk_02000548));
    gUnk_0200052C = 0;
    gUnk_02000550 = 0;
    gUnk_0200053C = 0;
    gUnk_02000554 = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08158C14);
#endif
