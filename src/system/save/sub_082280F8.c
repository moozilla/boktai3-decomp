#include "system/game_state/game_state.h"
s32 sub_082280F8(void)
{
    s32 best = 0;
    s32 max = -100;
    s32 i;

    for (i = 0; i < 5; i++) {
        s16 value = ((s16 *)gGameState->unk666)[i];

        if (value > max) {
            max = value;
            best = i;
        }
    }
    return best;
}
