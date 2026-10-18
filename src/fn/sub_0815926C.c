#include "global.h"

extern u32 gUnk_02000580[];
s32 Player_IsInActionTwelveOrThirtyEight(u8 *);

s32 Player_IsSlotInActionTwelveOrThirtyEight(s32 idx)
{
    return Player_IsInActionTwelveOrThirtyEight((u8 *)gUnk_02000580[idx]);
}
