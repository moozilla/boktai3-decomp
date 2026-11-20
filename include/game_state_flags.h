#ifndef GUARD_GAME_STATE_FLAGS_H
#define GUARD_GAME_STATE_FLAGS_H

#include "game_state.h"

static inline u8 GameState_TestWorldFlags(u32 mask)
{
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}

#endif
