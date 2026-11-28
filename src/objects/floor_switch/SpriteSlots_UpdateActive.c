#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/floor_switch/floor_switch_layout.h"

extern void *gUnk_086064AC[];
u32 FloorSwitch_UpdateRegionVisibility(struct FloorSwitch *);
s32 Callback_InvokeActor(void *, void *);

static inline u8 SpriteSlots_IsActive(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}

u32 SpriteSlots_UpdateActive(struct FloorSwitchManager *manager)
{
    struct FloorSwitch *switches = manager->switches;
    s32 i;

    if (!manager->used)
        return 0;
    for (i = 0; i < 8; i++) {
        struct FloorSwitch *floorSwitch = &switches[i];
        if (SpriteSlots_IsActive(&manager->used, i)) {
            if ((u8)FloorSwitch_UpdateRegionVisibility(floorSwitch))
                Callback_InvokeActor(floorSwitch, gUnk_086064AC[floorSwitch->field86]);
        }
    }
    return 0;
}
