#include "global.h"

// MATCHING_CONCESSION: register-pinning
#include "sun_orb_effect.h"

s32 RisingActor_AdvanceHeight(struct OrbitEffect *actor, s32 slot)
{
    register register s32 zero asm("r4");
    void *state;
    actor->scale += 0x2000;
    state = &actor->volley;
    zero = 0;
    ((struct OrbitVolleyState *)state)->lowered = zero;
    if (actor->scale > 0x5ffff) {
        s32 mask;
        state = &actor->collider;
        mask = ~4;
        ((struct ContactCollider *)state)->flags &= mask;
        SunOrb_SetUpdateCallback(&actor->params, OrbitEffect_FireVolley);
    }
    return 0;
}
