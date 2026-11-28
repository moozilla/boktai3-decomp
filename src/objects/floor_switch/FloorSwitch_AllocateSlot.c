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

struct FloorSwitch *FloorSwitch_AllocateSlot(struct FloorSwitchManager *manager)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (!Tst(&manager->used, i)) {
            struct FloorSwitch *floorSwitch;
            manager->used |= 1 << i;
            floorSwitch = &manager->switches[i];
            floorSwitch->slot = i;
            return floorSwitch;
        }
    }
    return 0;
}
