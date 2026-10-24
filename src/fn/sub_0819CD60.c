#include "global.h"
#ifdef NONMATCHING
// fuzzy: 81.9%
// Remaining differences: register allocation, stack lifetimes and instruction ordering.
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad[0x54];
    struct Vec position, velocity;
    u8 pad64[0x39c - 0x64];
    struct Vec target, steering;
};
extern const s16 gUnk_086149C4[];
u16 Sqrt(u32);
s32 Div(s32, s32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
static inline void Normalize(struct Vec *v)
{
    struct Vec *out = v;
    s32 speed = 12;
    u16 length = Sqrt(v->x * v->x + out->y * out->y + out->z * out->z);
    if (length) {
        v->x = Div(v->x * speed, length);
        out->y = Div(out->y * speed, length);
        out->z = Div(out->z * speed, length);
    }
}
void Hresvelgr_SteerTowardTarget(struct Actor *input)
{
    struct Actor *a = input;
    struct Vec delta, left, right;
    register struct Vec *v asm("r5") = &a->steering;
    register s32 y asm("r9");
    struct Vec *out = &delta;
    struct Vec *target = &a->target, *position = &a->position;
    delta.x = target->x - position->x;
    out->y = target->y - position->y;
    out->z = target->z - position->z;
    y = out->y;
    if ((u32)(y + 15) <= 30) {
        s32 value = -16;
        if (y > 0)
            value = 16;
        y = value;
    }
    y = y >= 0 ? y >> 4 : -(-y >> 4);
    Normalize(&delta);
    {
        s32 x = Scale(v->x * gUnk_086149C4[68] - v->z * gUnk_086149C4[4]);
        s32 yy = v->y;
        s32 z = Scale(v->x * gUnk_086149C4[4] + v->z * gUnk_086149C4[68]);
        struct Vec *q = &left;
        q->x = x;
        q->y = yy;
        q->z = z;
    }
    {
        s32 x = Scale(v->x * gUnk_086149C4[68] + v->z * gUnk_086149C4[4]);
        s32 yy = v->y;
        s32 z = Scale(-v->x * gUnk_086149C4[4] + v->z * gUnk_086149C4[68]);
        struct Vec *q = &right;
        q->x = x;
        q->y = yy;
        q->z = z;
    }
    if (v->x * left.x + v->z * left.z <= v->x * delta.x + v->z * delta.z) {
        v->x = delta.x;
        v->z = delta.z;
    } else if (left.x * delta.x + left.z * delta.z <= right.x * delta.x + right.z * delta.z)
        *v = right;
    else
        *v = left;
    Normalize(v);
    v->y = y;
    a->velocity = *v;
}

#else
INCLUDE_ASM("asm/nonmatching", Hresvelgr_SteerTowardTarget);
#endif
