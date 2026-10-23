// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct OrbitEffectPosition {
    s16 x, y, z;
};
struct OrbitEffectSteering {
    u8 unknown0[6];
    s8 direction;
    u8 unknown7;
    struct OrbitEffectPosition *target;
    u8 turnSpeed;
};
struct OrbitEffect {
    u8 unknown0[0x14];
    struct OrbitEffectSteering steering;
    u8 unknown21[0x50 - 0x24];
    struct OrbitEffectPosition position;
    u8 unknown56[0x8c - 0x58];
    u32 flags;
};
s32 Math_Atan2(s32, s32);
#define OrbitEffect_AdjustHeading sub_081FD664
void OrbitEffect_AdjustHeading(struct OrbitEffect *p)
{
    struct OrbitEffectSteering *params = &p->steering;
    if (params->target) {
        struct OrbitEffectPosition *pos = &p->position;
        u32 byte = (u8)Math_Atan2(params->target->x - pos->x, params->target->z - pos->z);
        u32 desired = byte;
        s32 x = p->steering.target->x - pos->x;
        s32 z = p->steering.target->z - pos->z;
        if (x * x + z * z <= 0x3ffff) {
            s32 heading = (s8)byte;
            heading += 0x40;
            params->direction = heading;
        } else {
            register u32 current asm("r2") = (u8)params->direction;
            s32 signedHeading = (s8)desired;
            s32 delta;
            register s32 limit asm("r0");
            delta = (s8)(signedHeading - current);
            limit = params->turnSpeed;
            if (delta > limit) {
                u32 nd = current + limit;
                params->direction = nd;
            } else if (delta < limit) {
                params->direction = current - limit;
            } else
                params->direction = desired;
        }
    }
    {
        u32 flag = 1;
        p->flags |= flag;
    }
}
