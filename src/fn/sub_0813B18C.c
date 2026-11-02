#include "global.h"
#include "contact_actor.h"

s32 Math_Atan2(s32, s32);
static inline s32 ImpactDirection(s32 angle)
{
    return (((angle & 255) >> 6) << 1) + 1;
}

s32 ActorDirection_ComputeRelativeCardinal(struct ContactCollider *a, struct ContactCollider *b)
{
    s32 angle;
    if (a->flags & 0x100) {
        u32 t = a->direction;
        angle = t + 0xe0;
        return ImpactDirection(angle);
    } else {
        s32 x = a->x - b->x;
        s32 z = a->z - b->z;
        if (x == 0 && z == 0) {
            angle = b->direction + 0xe0;
            return ImpactDirection(angle);
        } else {
            angle = Math_Atan2(x, z) + 0x60;
        }
    }
    return ImpactDirection(angle);
}
