#include "global.h"
#ifdef NONMATCHING
// fuzzy: 26.9%
#include "proximity_emitter.h"
u32 ProximityEmitter_IsTetherNearby(struct ProximityEmitter *emitter)
{
    struct ContactPosition *tether = TetherActor_GetPosition();
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
    /* The original retains this nonnull pointer in the square corners. */
    return (u32)tether;
}

#else
INCLUDE_ASM("asm/nonmatching", ProximityEmitter_IsTetherNearby);
#endif
