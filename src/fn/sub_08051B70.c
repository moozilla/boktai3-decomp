// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "proximity_emitter.h"
u32 ProximityEmitter_IsTetherNearby(struct ProximityEmitter *emitter)
{
    /* The original keeps the call result live in r0 and returns it when the
       tether is inside the box but outside the circle. */
    register u32 result asm("r0") = (u32)TetherActor_GetPosition();
    struct ContactPosition *tether = (struct ContactPosition *)result;
    if (!tether)
        goto outside;
    {
        s32 x = tether->x - emitter->position.x;
        s32 z;
        u16 dx, dz;
        if (x < 0)
            x = -x;
        dx = x;
        z = tether->z - emitter->position.z;
        if (z < 0)
            z = -z;
        dz = z;
        if (dx > 127 || dz > 127) {
        outside:
            return 0;
        }
        if (dx * dx + dz * dz <= 16383)
            return 1;
    }
    return result;
}
