#include "global.h"

// MATCHING_CONCESSION: register-pinning
#include "sun_orb_effect.h"

s32 RisingActor_AdvanceHeight(struct OrbitEffect *actor, s32 slot)
{
    register s32 zero asm("r4");
    register struct OrbitVolleyState *state asm("r2");
    actor->scale += 0x2000;
    state = &actor->volley;
    zero = 0;
    state->lowered = zero;
    if (actor->scale > 0x5ffff) {
        struct ContactCollider *collider = &actor->collider;
        s32 mask = ~4;
        collider->flags &= mask;
        SunOrb_SetUpdateCallback(&actor->params, OrbitEffect_FireVolley);
    }
    return 0;
}
