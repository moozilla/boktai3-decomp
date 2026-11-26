#include "engine/shared/global.h"
#include "engine/shared/ram.h"

s32 FloorSwitch_InitManager(u8 *obj)
{
    gFloorSwitchManager = obj;
    *(u32 *)(obj + 0x498) = 0;
    return 0;
}
