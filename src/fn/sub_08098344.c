#include "global.h"

#include "scene_actor_reset.h"

struct GhostCurveRoute {
    u8 unknown0[0x568];
    struct ContactPosition points[8];
    struct ContactPosition endpoint;
    u8 unknown5B0[0x5FC - 0x5B0];
    struct ContactPosition motion;
};
s32 Div(s32, s32);

static inline s32 Ghost_Shift10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}

static inline void Ghost_SubtractPoints(struct ContactPosition *out, struct ContactPosition *a,
                                        struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline void Ghost_InterpolatePoint(struct ContactPosition *out,
                                          struct ContactPosition *start,
                                          struct ContactPosition *end, s32 tx, s32 tz, s32 t)
{
    struct ContactPosition offset;
    struct ContactPosition acc;
    s32 q, p, twiceP, thriceQ, tangent, h11;
    s32 scaled, value;
    Ghost_SubtractPoints(&offset, end, start);
    ZeroFill8(&acc);
    q = Ghost_Shift10(t * t);
    p = Ghost_Shift10(q * t);
    twiceP = p * 2;
    thriceQ = q * 3;
    tangent = (p - 2 * q + t) * tz;
    h11 = p - q;
    scaled = ((twiceP - thriceQ + 1) * acc.x) * tx;
    value = (thriceQ - twiceP) * offset.x;
    value = scaled + value + tangent + h11;
    out->x = start->x + Ghost_Shift10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.y) * tx;
    value = (-twiceP + thriceQ) * offset.y;
    value = scaled + value + tangent + h11;
    out->y = start->y + Ghost_Shift10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.z) * tx;
    value = (-twiceP + thriceQ) * offset.z;
    value = scaled + value + tangent + h11;
    out->z = start->z + Ghost_Shift10(value);
}

void sub_08098344(struct SceneActorRegistryResetView *actor, struct ContactPosition *target)
{
    struct GhostCurveRoute *route = actor->context;
    struct ContactPosition start;
    struct ContactPosition end;
    s32 step, tx, tz, i;

    start = actor->body.position;
    end = *target;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&start = mask & *(u32 *)&start;
    }
    *(u32 *)&end &= 0xFFFF;
    step = Div(0x400, 8);
    tx = route->motion.x << 7;
    tz = route->motion.z << 7;
    for (i = 0; i < 8; i++) {
        s32 next = i + 1;
        struct ContactPosition *out = (struct ContactPosition *)(i * 8 + (u32)route);
        out = (struct ContactPosition *)((u8 *)out + (u32) & ((struct GhostCurveRoute *)0)->points);
        Ghost_InterpolatePoint(out, &start, &end, tx, tz, step * next);
    }
    route->endpoint = end;
}
