#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad[0x51];
    u8 heading;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0xa8 - 0x64];
    u8 facing4, facing8;
    u8 padAA[0x38a - 0xaa];
    u16 speed;
    u8 pad38C[0x39c - 0x38c];
    struct Vec target;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
static inline void SetVelocity(struct Vec *v, u32 angle, s32 speed)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * speed);
}
s32 Hresvelgr_UpdateTravel(struct Actor *a)
{
    struct Actor *saved = a;
    s32 result = 0;
    struct Vec *position = &a->position, *target = &a->target;
    s32 angle = Math_Atan2(target->x - position->x, target->z - position->z);
    s32 x = position->x - target->x, z = position->z - target->z;
    s32 old, delta;
    s32 next;
    if (x * x + z * z <= a->speed * a->speed) {
        SetVelocity(&a->velocity, a->heading, a->speed);
        result = 1;
    }
    old = a->heading;
    delta = (angle - old) & 255;
    if ((u32)(delta - 129) <= 124)
        next = old - 2;
    else {
        s32 diff = delta - 3;
        next = angle;
        if ((u32)diff <= 125)
            next = old + 2;
    }
    a->heading = next;
    {
        s32 value = (a->heading + 96) >> 6;
        u32 mask = 3;
        mask &= value;
        a->facing4 = mask;
    }
    {
        s32 value = (a->heading + 112) >> 5;
        u32 mask = 7;
        mask &= value;
        a->facing8 = mask;
    }
    SetVelocity(&a->velocity, next, saved->speed);
    return result;
}
