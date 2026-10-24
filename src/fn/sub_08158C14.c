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
// MATCHING_CONCESSION: register-pinning
// confidence=low: clears the link-mode shared arrays and counters in the movement-state tail.
#define Player_ResetLinkState sub_08158C14
void Player_ResetLinkState(void)
{
    s32 i;
    u16 *firstCounter, *secondCounter, *thirdCounter, *fourthCounter;
    s32 pairAddress;
    s32 pairBase;
    u16 zero;
    *(u16 *)0x02000544 = 0;
    for (i = 0; i < 4; i++) {
        gUnk_02000580[i] = 0;
        gUnk_02000508[i] = 0;
        gUnk_02000560[i] = 0;
    }

    gUnk_02000500 = 0;
    gUnk_02000520 = 0;
    gUnk_02000534 = 0;
    firstCounter = &gUnk_02000590;
    secondCounter = &gUnk_02000570;
    thirdCounter = &gUnk_02000528;
    fourthCounter = &gUnk_02000518;
    // Keep these addresses live together until the counter resets.
    asm("" : : "g"(firstCounter), "g"(secondCounter), "g"(thirdCounter), "g"(fourthCounter));
    *firstCounter = 0;
    *secondCounter = 0;
    *thirdCounter = 0;
    *fourthCounter = 0;
    pairBase = (s32)gUnk_02000548;
    pairAddress = pairBase;
    zero = 0;
    pairAddress += 2;
    do {
        *((u16 *)pairAddress) = zero;
        pairAddress -= 2;
    } while (pairAddress >= pairBase);
    gUnk_0200052C = 0;
    gUnk_02000550 = 0;
    gUnk_0200053C = 0;
    gUnk_02000554 = 0;
}
