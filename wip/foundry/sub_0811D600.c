#include "global.h"
#include "game_state.h"
#define GameRecords_UpdateMinimumRank sub_0811D600
#define gGameState                    gUnk_02000710
void sub_0811D600(u32 index, u32 rank)
{
    if (index <= 11 && rank <= 5) {
        struct GameState *state = gUnk_02000710;
        s16 *record;
        state = (void *)(index * 2 + (u32)state);
        record = (s16 *)((u8 *)state + 0x800);
        if (*record == -1 || state->unk800[0] > (s32)rank)
            *record = rank;
    }
}
