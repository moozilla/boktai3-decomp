#include "global.h"

#include "route_sprite_actor.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"
#include "movement_curve.h"
#include "vanargand_parts.h"

extern const s16 gUnk_086149C4[];
extern void *gUnknown_02000580;
s32 Mod(s32, s32);
s32 Div(s32, s32);
u8 *Path_GetPointData(u8 *);

struct WanderSource {
    u8 unknown0[0x30];
    struct ContactPosition position;
};

static inline struct ContactPosition *PrepareSourceDelta(void **source, struct ContactPosition *out,
                                                         struct RouteSpriteActor *actor)
{
    struct WanderSource *origin = *source;
    struct ContactPosition *target = &origin->position;
    Vanargand_Difference(out, &actor->sprite.position, target);
    return target;
}

static inline void f0806D438_Curve(struct ContactPosition *out, struct ContactPosition *start,
                                   struct ContactPosition *end, s32 tx, s32 tz, s32 t)
{
    struct ContactPosition offset;
    struct ContactPosition acc;
    s32 q, p, twiceP, thriceQ, tangent, h11;
    s32 scaled, value;
    Vanargand_Difference(&offset, end, start);
    ZeroFill8(&acc);
    q = MovementCurve_TruncateFixed10(t * t);
    p = MovementCurve_TruncateFixed10(q * t);
    twiceP = p * 2;
    thriceQ = q * 3;
    tangent = (p - 2 * q + t) * tz;
    h11 = p - q;
    scaled = ((twiceP - thriceQ + 1) * acc.x) * tx;
    value = (thriceQ - twiceP) * offset.x;
    value = scaled + value + tangent + h11;
    out->x = start->x + MovementCurve_TruncateFixed10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.y) * tx;
    value = (-twiceP + thriceQ) * offset.y;
    value = scaled + value + tangent + h11;
    out->y = start->y + MovementCurve_TruncateFixed10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.z) * tx;
    value = (-twiceP + thriceQ) * offset.z;
    value = scaled + value + tangent + h11;
    out->z = start->z + MovementCurve_TruncateFixed10(value);
}

void RouteSpriteActor_PlanWanderPath(struct RouteSpriteActor *actor)
{
    const u16 *table;
    struct RouteSpriteRoute *route = &actor->route;
    struct ContactPosition *target;
    struct ContactPosition polar;
    struct ContactPosition start;
    struct ContactPosition delta;
    s32 step, tx, tz, k;
    s32 radius;
    u8 index;
    s16 angle;

    table = gRandomTable;
    index = Mod(ParticleRandom_ReadSample(table), route->resource->pointCount);
    if (index == route->index) {
        index = Mod(index + 1, route->resource->pointCount);
    }
    route->index = index;
    route->ended = 0;
    route->current =
        (const struct RouteSpriteRoutePoint *)((u8 *)Path_GetPointData((u8 *)route->resource) +
                                               route->index * 8);

    target = PrepareSourceDelta(&gUnknown_02000580, &delta, actor);

    actor->elevatedPosition = *target;
    actor->prepared = 0;

    angle = ParticleRandom_ReadSample(table) & 255;
    radius = ParticleRandom_ReadSample(table) & 255;
    Vanargand_MakeRadialVector(&polar, angle, radius - 0x80);
    actor->elevatedPosition.x = actor->elevatedPosition.x + polar.x;
    actor->elevatedPosition.z = actor->elevatedPosition.z + polar.z;

    start = actor->sprite.position;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&start = mask & *(u32 *)&start;
    }
    actor->elevatedPosition.y = 0;

    step = Div(0x400, 12);
    tx = actor->target.x << 7;
    tz = actor->target.z << 7;

    for (k = 0; k < 12; k++) {
        s32 next = k + 1;
        struct ContactPosition *wp = (struct ContactPosition *)((u32)actor + k * 8);
        wp = (struct ContactPosition *)((u8 *)wp + (u32) &
                                        ((struct RouteSpriteActor *)0)->waypoints);
        f0806D438_Curve(wp, &start, &actor->elevatedPosition, tx, tz, step * next);
    }

    actor->endpoint = actor->elevatedPosition;
}
