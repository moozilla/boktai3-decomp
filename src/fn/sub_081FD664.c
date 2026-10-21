// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 97.5%
// foundry draft (worker best 92.4%); what differs is mostly register allocation and instruction order
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
            u32 current = (u8)params->direction;
            s32 delta = (s8)desired;
            register s32 limit asm("r0");
            delta = (s8)(delta - current);
            limit = params->turnSpeed;
            asm("" : "+r"(limit));
            if (delta > limit) {
                register u32 nd asm("r0") = current + limit;
                asm("" : "+r"(nd));
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
#else
INCLUDE_ASM("asm/nonmatching", OrbitEffect_AdjustHeading);
#endif
