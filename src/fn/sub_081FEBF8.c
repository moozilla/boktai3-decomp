#include "global.h"
#include "sun_orb_effect.h"
#include "ram.h"
void SunOrb_GetOrCreateManager(void);
s32 SunOrb_IsFadingOut(s32 i)
{
    struct OrbitEffectManager *manager = (struct OrbitEffectManager *)gSlotController;
    s32 result;
    if (!manager) {
        SunOrb_GetOrCreateManager();
        return -1;
    }
    result = 0;
    if (manager->slots[i].params.callback == OrbitEffect_UpdateContraction)
        result = 1;
    return result;
}
