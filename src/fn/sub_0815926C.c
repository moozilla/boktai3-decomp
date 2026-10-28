#include "global.h"
#include "player.h"

s32 Player_IsInActionTwelveOrThirtyEight(u8 *);

s32 Player_IsSlotInActionTwelveOrThirtyEight(s32 idx)
{
    return Player_IsInActionTwelveOrThirtyEight((u8 *)gPlayers[idx]);
}
