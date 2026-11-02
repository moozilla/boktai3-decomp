#include "global.h"
#include "ram.h"
#include "player.h"

static inline void Player_ClearRegisteredSlots(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        gPlayers[i] = NULL;
        gTrackedPlayerId[i] = 0;
        gPlayerSlotStatus[i] = 0;
    }
}

#define Player_ResetLinkState sub_08158C14
void Player_ResetLinkState(void)
{
    s32 i;

    gPlayerCount = 0;
    Player_ClearRegisteredSlots();
    gPlayerInteractionMode = 0;
    gPlayerScriptState = 0;
    gPlayerPassageState = 0;
    gPlayerEffectPendingCount = 0;
    gPlayerSolarActionPending = 0;
    gPlayerActionPending = 0;
    gPlayerCueTimer = 0;
    for (i = 0; i < 2; i++)
        gPlayerInitialSlotValues[i] = 0;
    gPlayerDirectionLatch = 0;
    gPlayerBackgroundState = 0;
    gPlayerBackgroundMode = 0;
    gPlayerAbilityLatched = 0;
}
