#include "system/game_state/game_state.h"
s32 Div(s32, s32);
/* Bucket the share of mode-two input ticks in all input ticks (in 1/128ths):
 * above 70 -> 0, 59-70 -> 2, else 1; stored in unk660. */
s32 GameState_UpdateCounterBalanceRank(void)
{
    s32 total, share, rank;
    if (gGameState->modeTwoInputTicks == 0)
        gGameState->modeTwoInputTicks = 1;
    if (gGameState->modeOneInputTicks == 0)
        gGameState->modeOneInputTicks = 1;
    total = gGameState->modeOneInputTicks + gGameState->modeTwoInputTicks;
    if (total == 0)
        total = 1;
    share = Div(gGameState->modeTwoInputTicks << 7, total);
    rank = 0;
    if (share <= 70) {
        rank = 1;
        if (share > 58)
            rank = 2;
    }
    gGameState->unk660 = rank;
    return rank;
}
