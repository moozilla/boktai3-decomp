#include "global.h"
#include "game_state.h"
#include "engine.h"
extern u32 gUnk_030053F0;
s32 StateFlags_TestIndexedFlag(s32 index)
{
    if ((u32)index <= 11) {
        u32 mask = 0x800;
        if (((gEngineFlags | gUnk_030053F0) & mask) && index > 5)
            goto enabled;
        if ((gGameState->counters628[3] >> index) & 1)
            goto enabled;
    }
    return 0;
enabled:
    return 1;
}
