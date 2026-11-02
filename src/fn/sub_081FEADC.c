#include "global.h"
#include "sun_orb_effect.h"
#include "ram.h"
void SunOrb_GetOrCreateManager(void);
s32 SlotController_RequestReset(s32 index)
{
    struct OrbitEffectManager *controller = (struct OrbitEffectManager *)gSlotController;
    if (!controller) {
        SunOrb_GetOrCreateManager();
        return -1;
    }
    if (!(controller->activeSlots & (1 << index)))
        return -1;
    if (controller->slots[index].volley.lowered != 1)
        SunOrb_SetUpdateCallback(&controller->slots[index].params, RisingActor_LowerHeight);
    return 0;
}
