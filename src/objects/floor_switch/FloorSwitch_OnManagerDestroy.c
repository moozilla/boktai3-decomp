#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/floor_switch/floor_switch_layout.h"

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

u32 FloorSwitch_OnManagerDestroy(struct FloorSwitchManager *manager)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (Tst(&manager->used, i))
            Sprite_Remove(&manager->switches[i]);
    }
    gFloorSwitchManager = 0;
    return 0;
}
