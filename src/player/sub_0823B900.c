#include "engine/shared/global.h"
#include "engine/shared/game_state_flags.h"
u32 Player_IsFlaggedWithGlobalModifier08(u32 x)
{
    if (x & 0x80) {
        if (GameState_TestWorldFlags(8))
            return 1;
    }
    return 0;
}
