#include "global.h"
#include "game_state_flags.h"
u32 Player_IsFlaggedWithoutGlobalModifiers(u32 x)
{
    if (x & 0x80) {
        if (!GameState_TestWorldFlags(0x18))
            return 1;
    }
    return 0;
}
