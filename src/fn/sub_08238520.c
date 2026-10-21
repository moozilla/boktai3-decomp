// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_actor.h"

s32 Math_Atan2(s32, s32);
s32 ContactCollider_GetImpactDirection(struct ContactCollider *a, struct ContactCollider *b)
{
    s32 angle;
    s32 result;
    if (a->flags & 0x100) {
        u32 t = a->direction;
        angle = t + 0xe0;
        asm("" : "+r"(angle));
        goto done;
    } else {
        s32 x = a->x - b->x;
        s32 z = a->z - b->z;
        if (x == 0 && z == 0) {
            angle = b->direction + 0xe0;
        } else {
            angle = Math_Atan2(x, z) + 0x60;
        }
    }
done:
    angle &= 0xff;
    return ((angle >> 6) << 1) + 1;
}
