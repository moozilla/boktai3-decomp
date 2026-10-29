#include "global.h"

struct BeePoint {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct BeeRoute {
    u8 unknown0[0x3E0];
    struct BeePoint points[8];
    struct BeePoint endpoint;
    u8 unknown428[0x480 - 0x428];
    struct BeePoint motion;
};
struct BeeActor {
    u8 unknown0[0x5C];
    struct BeePoint position;
    u8 unknown64[0x3D0 - 0x64];
    struct BeeRoute *route;
};
s32 Div(s32, s32);

static inline s32 Bee_Shift10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}

static inline void Bee_SubtractPoints(struct BeePoint *out, struct BeePoint *a, struct BeePoint *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline void Bee_InterpolatePoint(struct BeePoint *out, struct BeePoint *start,
                                        struct BeePoint *end, s32 tx, s32 tz, s32 t)
{
    struct BeePoint offset;
    struct BeePoint acc;
    s32 q, p, twiceP, thriceQ, tangent, h11;
    s32 scaled, value;
    Bee_SubtractPoints(&offset, end, start);
    ZeroFill8(&acc);
    q = Bee_Shift10(t * t);
    p = Bee_Shift10(q * t);
    twiceP = p * 2;
    thriceQ = q * 3;
    tangent = (p - 2 * q + t) * tz;
    h11 = p - q;
    scaled = ((twiceP - thriceQ + 1) * acc.x) * tx;
    value = (thriceQ - twiceP) * offset.x;
    value = scaled + value + tangent + h11;
    out->x = start->x + Bee_Shift10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.y) * tx;
    value = (-twiceP + thriceQ) * offset.y;
    value = scaled + value + tangent + h11;
    out->y = start->y + Bee_Shift10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.z) * tx;
    value = (-twiceP + thriceQ) * offset.z;
    value = scaled + value + tangent + h11;
    out->z = start->z + Bee_Shift10(value);
}

void sub_0809345C(struct BeeActor *actor, struct BeePoint *target)
{
    struct BeeRoute *route = actor->route;
    struct BeePoint start;
    struct BeePoint end;
    s32 step, tx, tz, i;

    start = actor->position;
    end = *target;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&start = mask & *(u32 *)&start;
    }
    *(u32 *)&end &= 0xFFFF;
    step = Div(0x400, 8);
    tx = route->motion.x << 4;
    tz = route->motion.z << 4;
    for (i = 0; i < 8; i++) {
        s32 next = i + 1;
        struct BeePoint *out = (struct BeePoint *)(i * 8 + (u32)route);
        out = (struct BeePoint *)((u8 *)out + (u32) & ((struct BeeRoute *)0)->points);
        Bee_InterpolatePoint(out, &start, &end, tx, tz, step * next);
    }
    route->endpoint = end;
}
