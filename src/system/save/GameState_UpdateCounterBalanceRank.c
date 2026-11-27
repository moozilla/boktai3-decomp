#include "global.h"
extern u8 *gGameState;
s32 Div(s32, s32);
/* Bucket the share of counter 0x638 in 0x638 + 0x63C (in 1/128ths):
 * above 70 -> 0, 59-70 -> 2, else 1; stored at 0x660. */
s32 GameState_UpdateCounterBalanceRank(void)
{
    s32 total, share, rank;
    if (*(u32 *)(gGameState + 0x638) == 0)
        *(u32 *)(gGameState + 0x638) = 1;
    if (*(u32 *)(gGameState + 0x63c) == 0)
        *(u32 *)(gGameState + 0x63c) = 1;
    total = *(u32 *)(gGameState + 0x63c) + *(u32 *)(gGameState + 0x638);
    if (total == 0)
        total = 1;
    share = Div(*(u32 *)(gGameState + 0x638) << 7, total);
    rank = 0;
    if (share <= 70) {
        rank = 1;
        if (share > 58)
            rank = 2;
    }
    *(u16 *)(gGameState + 0x660) = rank;
    return rank;
}
