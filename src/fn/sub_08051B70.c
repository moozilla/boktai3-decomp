#include "global.h"
#include "proximity_emitter.h"
u32 ProximityEmitter_IsTetherNearby(struct ProximityEmitter *emitter)
{
    /* The original keeps the call result live in r0 and returns it when the
       tether is inside the box but outside the circle. */
    u32 result = (u32)TetherActor_GetPosition();
    s32 k2 = 0;
    struct ContactPosition *tether = (struct ContactPosition *)result;
    if (!tether) {
        goto outside;
    }
    {
        s32 x = tether->x - emitter->position.x;
        s32 z;
        u16 dx;
        u16 dz;
        if (x < 0) {
            x = -x;
        }
        dx = x;
        z = tether->z - emitter->position.z;
        if (z < k2) {
            z = -z;
        }
        dz = z;
        if (dx > 127 || dz > 127) {
        outside:
            return 0;
        }
        if (((dx * dx) + (dz * dz)) <= 16383) {
            return 1;
            return result;
        }
    }
}
