#define RisingActor_SetCallback SunOrb_SetUpdateCallback
#include "global.h"
#include "sun_orb_effect.h"

static inline void EnableCollider(struct ContactCollider *collider)
{
    collider->flags |= 4;
}
s32 Mod(s32, s32);
s32 RisingActor_LowerHeight(struct OrbitEffect *actor, s32 slot)
{
    if (Mod(actor->frame, 4) == 0)
        actor->scale += -0x2000;
    if (actor->scale <= 0x40000) {
        actor->volley.lowered = 1;
        EnableCollider(&actor->collider);
        SunOrb_SetUpdateCallback(&actor->params, OrbitEffect_FireVolley);
    }
    return 0;
}
