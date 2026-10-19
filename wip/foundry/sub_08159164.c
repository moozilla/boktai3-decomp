#define PlayerRewards_AddTotal sub_08159164
#define gPlayerSlots           gUnk_02000580
#define gGameState             gUnk_02000710
#include "global.h"
#include "game_state.h"
#include "player.h"
static inline s32 HasBonus(u32 mask)
{
    u32 *flags;
    if (!gPlayerSlots[0])
        return 0;
    flags = &gPlayerSlots[0]->statBonusFlags;
    if (mask & *flags)
        return 1;
    return 0;
}

s32 Div(s32, s32);
void sub_08159164(s32 amount)
{
    if (gGameState->form != 6) {

        if (HasBonus(0x8000)) {
            if (HasBonus(0x10000))
                goto add;
        }
        {
            if (HasBonus(0x8000))
                amount = (amount * 3) >> 1;
            else if (HasBonus(0x10000))
                amount = Div(amount, 2);
        }
    add:
        *(s32 *)((u8 *)gGameState + 0x48) += amount;
        if (gGameState->rewardTotal > 999998)
            *(s32 *)((u8 *)gGameState + 0x48) = 999999;
    }
}
