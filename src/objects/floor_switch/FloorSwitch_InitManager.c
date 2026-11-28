#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/floor_switch/floor_switch_layout.h"

s32 FloorSwitch_InitManager(struct FloorSwitchManager *manager)
{
    gFloorSwitchManager = (u32)manager;
    manager->used = 0;
    return 0;
}
