#include "global.h"

struct Vector {
    s16 x, y, z;
};

struct VanargandProjectionSource {
    u8 unk_00[0x54];
    struct Vector position;
    u8 unk_5C[0x110 - 0x5C];
    struct Vector *targetPosition;
};

s32 __divsi3(s32, s32);
u16 Sqrt(u32);

static inline s32 Shift(s32 v, s32 n)
{
    s32 r;
    if (v >= 0)
        r = v >> n;
    else
        r = -((-v) >> n);
    return r;
}

static inline void Project(struct Vector *out, struct Vector *p)
{
    s32 x = p->x;
    s32 z = p->z;
    s32 sum, y;
    out->x = Shift((x - z) * 3, 5);
    sum = Shift((x + z) * 3, 5);
    y = Shift(p->y * 3, 5);
    out->y = sum - y;
    out->z = sum + y;
}

static inline void SetVector(struct Vector *out, s32 x, s32 y, s32 z)
{
    out->x = x;
    out->y = y;
    out->z = z;
}

static inline void Unproject(struct Vector *out, struct Vector *p, s32 y)
{
    s32 x = p->x;
    s32 h = p->y;
    s32 v;
    v = __divsi3((x + h) << 5, 3) + y;
    out->x = Shift(v, 1);
    out->y = y;
    v = __divsi3((h - x) << 5, 3) + y;
    out->z = Shift(v, 1);
}

// Tentative: measures target separation along the projected horizontal axis.

u32 Vanargand_GetTargetLateralDistance(struct VanargandProjectionSource *s)
{
    struct Vector origin, target, difference, world;
    Project(&origin, &s->position);
    {
        struct Vector **slot = &s->targetPosition;
        Project(&target, *slot);
    }
    SetVector(&difference, target.x - origin.x, 0, 0);
    Unproject(&world, &difference, 0);
    return Sqrt(world.x * world.x + world.z * world.z);
}
