#include "global.h"

#include "sun_orb_effect.h"

s32 RisingActor_AdvanceHeight(struct OrbitEffect *actor, s32 slot)
{
    union {
        s32 value;
        s16 halves[2];
    } zero;
    void *state;
    actor->scale += 0x2000;
    state = &actor->volley;
    zero.halves[0] = 0;
    zero.halves[1] = 0;
    ((struct OrbitVolleyState *)state)->lowered = zero.value;
    if (actor->scale > 0x5ffff) {
        s32 mask;
        state = &actor->collider;
        mask = ~4;
        ((struct ContactCollider *)state)->flags &= mask;
        SunOrb_SetUpdateCallback(&actor->params, OrbitEffect_FireVolley);
    }
    return 0;
}
