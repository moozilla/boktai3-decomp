#include "sun_orb_effect.h"
struct OrbitEffectPosition *SunOrb_GetPositionFromManager(struct OrbitEffectManager *p, s32 i)
{
    struct OrbitEffectPosition *r;
    if (p != 0)
        r = &p->slots[i].sprite.position;
    else
        r = 0;
    return r;
}
