#include "global.h"
extern u16 gPlayerCount;
extern u16 gPlayerInteractionMode;
extern u16 gPlayerScriptState;
extern u16 gPlayerPassageState;
extern u16 gPlayerEffectPendingCount;
extern u16 gPlayerSolarActionPending;
extern u16 gPlayerActionPending;
extern u16 gPlayerCueTimer;
extern u32 gUnk_02000560[4];
extern u32 gUnk_02000580[4];
extern u16 gTrackedPlayerId[4];
extern u16 gPlayerInitialSlotValues[2];
extern u16 gPlayerDirectionLatch, gPlayerBackgroundState, gPlayerBackgroundMode,
    gPlayerAbilityLatched;
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
        gTrackedPlayerId[i] = 0;
        gUnk_02000560[i] = 0;
    }

    gPlayerInteractionMode = 0;
    gPlayerScriptState = 0;
    gPlayerPassageState = 0;
    firstCounter = &gPlayerEffectPendingCount;
    secondCounter = &gPlayerSolarActionPending;
    thirdCounter = &gPlayerActionPending;
    fourthCounter = &gPlayerCueTimer;
    // Keep these addresses live together until the counter resets.
    asm("" : : "g"(firstCounter), "g"(secondCounter), "g"(thirdCounter), "g"(fourthCounter));
    *firstCounter = 0;
    *secondCounter = 0;
    *thirdCounter = 0;
    *fourthCounter = 0;
    pairBase = (s32)gPlayerInitialSlotValues;
    pairAddress = pairBase;
    zero = 0;
    pairAddress += 2;
    do {
        *((u16 *)pairAddress) = zero;
        pairAddress -= 2;
    } while (pairAddress >= pairBase);
    gPlayerDirectionLatch = 0;
    gPlayerBackgroundState = 0;
    gPlayerBackgroundMode = 0;
    gPlayerAbilityLatched = 0;
}
