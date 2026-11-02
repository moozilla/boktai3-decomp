#include "sun_orb_effect.h"
#include "ram.h"
void SunOrb_GetOrCreateManager(void);
struct OrbitEffectPosition *SunOrb_GetPosition(s32 i)
{
    struct OrbitEffectManager *g = (struct OrbitEffectManager *)gSlotController;
    struct OrbitEffectPosition *r;
    if (g != 0)
        r = &g->slots[i].sprite.position;
    else {
        SunOrb_GetOrCreateManager();
        r = 0;
    }
    return r;
}
