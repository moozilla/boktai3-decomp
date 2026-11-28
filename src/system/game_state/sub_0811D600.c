#include "global.h"
#include "system/game_state/game_state.h"

void sub_0811D600(u32 challenge, u32 rank)
{
    if (challenge <= 11 && rank <= 5) {
        if (*(challenge + gGameState->unk800) == -1)
            *(challenge + gGameState->unk800) = rank;
        else if (*(challenge + gGameState->unk800) > (s32)rank)
            *(challenge + gGameState->unk800) = rank;
    }
}
