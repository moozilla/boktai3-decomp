#include "global.h"
#include "system/game_state/game_state_flags.h"
u32 Player_IsStatusBit4ActionActive(u32 x)
{
    if (x & 0x80) {
        if (GameState_TestWorldFlags(0x10))
            return 1;
    }
    return 0;
}
