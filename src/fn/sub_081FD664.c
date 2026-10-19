#include "global.h"
#ifdef NONMATCHING
// fuzzy: 92.4%
// foundry draft (worker best 92.4%); what differs is mostly register allocation and instruction order
#define OrbitEffect_AdjustHeading sub_081FD664
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
void sub_081FD664(struct OrbitEffect *p)
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
            s32 step;
            s32 limit;
            limit = params->turnSpeed;
            delta = (s8)(delta - current);
            step = limit;
            if (delta > limit) {
                delta = current + step;
                params->direction = delta;
            } else if (delta < limit) {
                delta = current - step;
                params->direction = delta;
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
INCLUDE_ASM("asm/nonmatching", sub_081FD664);
#endif
