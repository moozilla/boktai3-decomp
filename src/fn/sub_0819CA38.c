// MATCHING_CONCESSION: register-pinning
#include "global.h"

// foundry draft (worker best 84.8%); what differs is mostly register allocation and instruction order
#define OffsetActor_SetTravelTarget Hresvelgr_RefreshTarget
struct OffsetActorVector {
    s16 x, y, z, pad;
};
struct OffsetActor {
    u8 pad0[0x51];
    u8 facing;
    u8 pad52[2];
    struct OffsetActorVector position;
    u8 pad5C[0xA8 - 0x5C];
    u8 facing4, facing8;
    u8 padAA[0x110 - 0xAA];
    struct OffsetActorVector *origin;
    u8 pad114[0x32B - 0x114];
    u8 side;
    u8 pad32C[0x38A - 0x32C];
    s16 travel;
    u8 pad38C[0x39C - 0x38C];
    struct OffsetActorVector target;
};
struct OffsetActorAngles {
    s32 a[2];
};
extern const struct OffsetActorAngles gUnk_0824F724;
extern const s16 gUnk_086149C4[];
u16 Sqrt(u32);
static inline s32 Scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -(-v >> 12);
}
static inline void Polar(struct OffsetActorVector *out, s32 angle, s32 radius)
{
    out->x = Scale(gUnk_086149C4[(angle + 0x40) & 255] * radius);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void Add(struct OffsetActorVector *a, struct OffsetActorVector *b)
{
    a->x = b->x + a->x;
    a->y = a->y + b->y;
    a->z = b->z + a->z;
}
static inline void Subtract(struct OffsetActorVector *a, struct OffsetActorVector *b)
{
    a->x = a->x - b->x;
    a->y = a->y - b->y;
    a->z = a->z - b->z;
}
s32 Hresvelgr_RefreshTarget(struct OffsetActor *p)
{
    struct OffsetActorAngles angles = gUnk_0824F724;
    struct OffsetActorVector v;
    struct OffsetActor *obj = p;
    Polar(&v, (p->side << 6) | 0x80, 0x300);
    {
        struct OffsetActorVector *o = obj->origin;
        struct OffsetActorVector *vp;
        vp = &v;
        Add(vp, o);
    }
    p->target = v;
    obj->facing = angles.a[p->side];
    {
        u32 v = (obj->facing + 0x60) >> 6;
        u32 m = 3;
        m &= v;
        obj->facing4 = m;
    }
    {
        u32 v = (obj->facing + 0x70) >> 5;
        u32 m = 7;
        m &= v;
        obj->facing8 = m;
    }
    Subtract(&v, &obj->position);
    {
        s32 result = Sqrt(v.x * v.x + v.z * v.z) * 0x274 / 0x6400;
        register s32 t asm("r2") = result;
        p->travel = t;
        return result;
    }
}
