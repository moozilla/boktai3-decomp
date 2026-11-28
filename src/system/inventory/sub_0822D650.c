#include "system/game_state/game_state.h"

u32 sub_0822D650(s32 bit)
{
    u32 *flags;

    if (bit < 32) {
        flags = &gGameState->flags72C;
    } else {
        flags = &gGameState->flags730;
        bit -= 32;
    }
    return *flags & (1 << bit);
}
