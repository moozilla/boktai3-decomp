#include "engine/shared/global.h"
#include "engine/shared/game_state_flags.h"
u32 sub_0823B8C4(u32 flags)
{
    if ((flags & 0x80) && GameState_TestWorldFlags(0x4000))
        return 1;
    return 0;
}
