#include "global.h"
#include "contact_actor.h"

#ifdef NONMATCHING
// fuzzy: 95.7%
// foundry draft (worker best 95.7%); what differs is mostly register allocation and instruction order
#define ContactCollider_GetImpactDirection sub_08238520
s32 Math_Atan2(s32, s32);
s32 sub_08238520(struct ContactCollider *a, struct ContactCollider *b)
{
    s32 angle;
    s32 result;
    if (a->flags & 0x100) {
        angle = a->direction;
        angle += 0xe0;
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
#else
INCLUDE_ASM("asm/nonmatching", sub_08238520);
#endif
