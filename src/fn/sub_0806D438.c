#include "global.h"

#include "route_sprite_actor.h"

/* confidence=low: picks the next route point, then builds the 12-point wander spline toward the source position */

/* confidence=low: picks the next route point, then builds the 12-point wander spline toward the source position */
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
extern void *gUnknown_02000580;
s32 Mod(s32, s32);
s32 Div(s32, s32);
u8 *Path_GetPointData(u8 *);

static inline s32 f0806D438_T10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}

static inline s32 f0806D438_T12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline u32 f0806D438_NextRandom(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(const u16 *)((u32)gUnk_0203B400 + gUnk_03005308 * 2);
}

static inline u32 f0806D438_NextByte(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(const u8 *)((u32)gUnk_0203B400 + gUnk_03005308 * 2);
}

static inline void f0806D438_SubVec(struct ContactPosition *out, struct ContactPosition *a,
                                    struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline void f0806D438_Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = f0806D438_T12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = f0806D438_T12(gUnk_086149C4[angle & 255] * radius);
}

/* confidence=low: picks the next route point, then builds the 12-point wander spline toward the source position */
#ifdef NONMATCHING
// fuzzy: 46.7%
// scope shape 175 (differences once low registers are set aside)
void RouteSpriteActor_PlanWanderPath(struct RouteSpriteActor *actor)
{
    struct RouteSpriteRoute *route = &actor->route;
    struct ContactPosition *target;
    struct ContactPosition *elevated;
    struct ContactPosition polar;
    struct ContactPosition start;
    struct ContactPosition offset;
    struct ContactPosition acc;
    struct ContactPosition delta;
    s32 step, tx, tz, k;
    s32 radius, t, q, p, h00, h01, h11, tangent;
    u8 index, angle;

    index = Mod(f0806D438_NextRandom(), route->resource->pointCount);
    if (index == route->index) {
        index = Mod(index + 1, route->resource->pointCount);
    }
    route->index = index;
    route->ended = 0;
    route->current =
        (const struct RouteSpriteRoutePoint *)((u8 *)Path_GetPointData((u8 *)route->resource) +
                                               route->index * 8);

    target = (struct ContactPosition *)((u8 *)gUnknown_02000580 + 0x30);
    f0806D438_SubVec(&delta, &actor->sprite.position, target);

    elevated = &actor->elevatedPosition;
    *elevated = *target;
    actor->prepared = 0;

    angle = f0806D438_NextByte();
    radius = f0806D438_NextByte() - 0x80;
    f0806D438_Polar(&polar, angle, radius);
    elevated->x = elevated->x + polar.x;
    elevated->y = 0;
    elevated->z = elevated->z + polar.z;

    start = actor->sprite.position;
    *(u32 *)&start &= 0xFFFF;

    step = Div(0x400, 12);
    tx = actor->target.x << 7;
    tz = actor->target.z << 7;

    for (k = 0; k < 12; k++) {
        struct ContactPosition *wp = &actor->waypoints[k];

        t = step * (k + 1);
        offset.x = elevated->x - start.x;
        offset.y = elevated->y - start.y;
        offset.z = elevated->z - start.z;
        ZeroFill8(&acc);
        q = f0806D438_T10(t * t);
        p = f0806D438_T10(q * t);
        tangent = (p - 2 * q + t) * tz;
        h11 = p - q;
        h00 = 2 * p - 3 * q + 1;
        h01 = 3 * q - 2 * p;
        wp->x = start.x + f0806D438_T10(tx * (acc.x * h00) + h01 * offset.x + tangent + h11);
        wp->y = start.y + f0806D438_T10(tx * (acc.y * h00) + h01 * offset.y + tangent + h11);
        wp->z = start.z + f0806D438_T10(tx * (acc.z * h00) + h01 * offset.z + tangent + h11);
    }

    actor->endpoint = *elevated;
}
#else
INCLUDE_ASM("asm/nonmatching", RouteSpriteActor_PlanWanderPath);
#endif
