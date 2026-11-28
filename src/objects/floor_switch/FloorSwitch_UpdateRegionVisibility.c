#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/floor_switch/floor_switch_layout.h"
#include "system/game_state/game_state.h"

static inline u8 Eq(u32 a, u32 b)
{
    if (a == b)
        return TRUE;
    return FALSE;
}

u32 FloorSwitch_UpdateRegionVisibility(struct FloorSwitch *floorSwitch)
{
    u32 region = floorSwitch->region;

    if (Eq(gGameState->region, region)) {
        floorSwitch->flags &= ~1;
        return 1;
    }
    floorSwitch->flags |= 1;
    return 0;
}
