#include "global.h"
#include "sun_orb_effect.h"
#include "ram.h"
void SunOrb_GetOrCreateManager(void);
s32 RisingActorManager_BeginRaise(s32 index)
{
    struct OrbitEffectManager *manager = (struct OrbitEffectManager *)gSlotController;
    if (!manager) {
        SunOrb_GetOrCreateManager();
        return -1;
    }
    if (!(manager->activeSlots & (1 << index)))
        return -1;
    if (manager->slots[index].volley.lowered)
        SunOrb_SetUpdateCallback(&manager->slots[index].params, RisingActor_AdvanceHeight);
    return 0;
}
