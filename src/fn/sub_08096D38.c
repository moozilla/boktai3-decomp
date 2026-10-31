#include "global.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
struct GhostPlayer {
    u8 pad[0x30];
    struct ContactPosition position;
};
void Ghost_EmitAfterimage(struct SceneActorRegistryResetView *);
void sub_08098344(void *, void *);
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
struct GhostRoute {
    u8 pad[0x558];
    u32 flags;
    u32 distance;
    u8 pad560[6];
    u16 speed;
    struct ContactPosition points[9];
    u8 index;
    u8 pad5B1[0x5FC - 0x5B1];
    struct ContactPosition motion;
};
static inline s32 Ghost_ScaleFixed12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline void Ghost_MakePolarMotion(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Ghost_ScaleFixed12(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Ghost_ScaleFixed12(gUnk_086149C4[angle & 255] * speed);
}
static inline void Ghost_SubtractPosition(struct ContactPosition *out,
                                          const struct ContactPosition *from,
                                          const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline u16 Ghost_NextRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + RANDOM_TABLE_ADDR);
}
static inline u16 Ghost_NextMotionRandom(void)
{
    u32 offset;
    const u16 *table;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    return *(const u16 *)(offset + (u32)table);
}
static inline u32 Ghost_OffsetHeading(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}
void SceneActorControl_OnIdle(void);
static inline void Ghost_InstallControl(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    a->field2B2 = 1;
    zero = 0;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}
static inline void Ghost_SetState(struct SceneActorRegistryResetView *a, u8 phase, u8 behavior)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline void Ghost_AddPosition(struct ContactPosition *out,
                                     const struct ContactPosition *from)
{
    out->x = out->x + from->x;
    out->y = out->y + from->y;
    out->z = out->z + from->z;
}
static inline s32 Ghost_QuarterDistance(s32 n)
{
    if (n >= 0)
        return n >> 2;
    return -(-n >> 2);
}
static inline void Ghost_SetColliderFlags(struct ContactCollider *c, u32 bits)
{
    c->flags |= bits;
}
inline static void Ghost_DisableCollider(struct ContactCollider *c)
{
    struct SignedFlags {
        u8 pad[6];
        s32 bits : 16;
    };
    {
        u8 mask = 4;
        ((struct SignedFlags *)c)->bits &= ~mask;
    }
}

static inline u8 Ghost_TestRouteFlags(struct GhostRoute *route, u32 mask)
{
    u32 flags = route->flags;
    if (flags & mask)
        return 1;
    return 0;
}
static inline void Ghost_ClearMotionFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u16 *flags = &actor->flags2D4;
    u32 mask = ~bits;
    *flags &= mask;
}
static inline void Ghost_ClearRouteFlags(struct GhostRoute *route, u32 bits)
{
    u32 mask = ~bits;
    route->flags &= mask;
}
static inline s32 Ghost_LengthSquared(const struct ContactPosition *position)
{
    return position->x * position->x + position->z * position->z;
}
#define GHOST_WRITE_COMPONENT(out_, field_, value_)                                                \
    do {                                                                                           \
        (out_)->field_ = (value_);                                                                 \
    } while (0)
#define GHOST_SET_VERTICAL(out_, height_)                                                          \
    do {                                                                                           \
        GHOST_WRITE_COMPONENT(out_, y, height_);                                                   \
    } while (0)
static inline void Ghost_MakeRetreatOffset(struct ContactPosition *out, u32 angle)
{
    s32 x, z;
    s32 speed = 728;
    u32 zero;
    x = Ghost_ScaleFixed12(gUnk_086149C4[(angle + 64) & 255] * speed);
    zero = 0;
    GHOST_WRITE_COMPONENT(out, x, x);
    GHOST_SET_VERTICAL(out, zero);
    z = Ghost_ScaleFixed12(gUnk_086149C4[angle & 255] * speed);
    GHOST_WRITE_COMPONENT(out, z, z);
}

// confidence=low: follows a finite curved route toward or away from the player.
void Ghost_AdvanceApproachRoute(struct SceneActorRegistryResetView *actor)
{
    struct GhostRoute *route = actor->context;
    struct ContactPosition target;
    struct ContactPosition delta;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        Ghost_InstallControl(actor, actor->entryTarget228, 1);
        route->index = 0;
        if (Ghost_TestRouteFlags(route, 4)) {
            u8 reverseBias = 128;
            Ghost_MakeRetreatOffset(
                &target,
                (actor->tracking.heading + ((Ghost_NextRandom() & 63) + reverseBias)) & 255);
            Ghost_AddPosition(&target, &actor->body.position);
        } else {
            target = (*(struct GhostPlayer **)gPlayers)->position;
        }
        Ghost_SubtractPosition(&delta, &actor->body.position, &target);
        route->distance = Ghost_QuarterDistance(Ghost_LengthSquared(&delta));
        Ghost_MakePolarMotion(
            &route->motion,
            Ghost_OffsetHeading(Ghost_NextMotionRandom(), &actor->tracking.heading) & 255, 32);
        sub_08098344(actor, &target);
        route->speed = 1;
        {
            u32 mask = 0x100;
            actor->flags2D4 = mask | actor->flags2D4;
        }
        actor->field2C2 = 32;
    }
    Ghost_SubtractPosition(&delta, &actor->body.position, &route->points[route->index]);
    if (!Ghost_TestRouteFlags(route, 4))
        Ghost_DisableCollider(&actor->colliderB);
    Ghost_EmitAfterimage(actor);
    if ((Ghost_LengthSquared(&delta)) > 1024) {
        actor->body.direction = Math_Atan2(route->points[route->index].x - actor->body.position.x,
                                           route->points[route->index].z - actor->body.position.z);
        Ghost_MakePolarMotion(&delta, actor->body.direction, route->speed);
        actor->body.motion.x += delta.x;
        actor->body.motion.z += delta.z;
        route->motion = actor->body.motion;
    } else if ((++route->index) > 8) {
        actor->body.position.x = route->points[8].x;
        actor->body.position.z = route->points[8].z;
        Ghost_ClearMotionFlags(actor, 0x100);
        Ghost_SetColliderFlags(&actor->colliderB, 4);
        if (Ghost_TestRouteFlags(route, 4)) {
            Ghost_SetState(actor, 0, 0);
            Ghost_InstallControl(actor, SceneActorControl_OnIdle, 16);
            Ghost_ClearRouteFlags(route, 4);
        } else {
            Ghost_SetState(actor, 2, 1);
            Ghost_InstallControl(actor, SceneActorControl_OnIdle, 16);
        }
        return;
    }
    if (actor->paletteOffset == 0) {
        if ((Mod(actor->counter2AC, 8) == 0) && (route->speed <= 19))
            ++route->speed;
    } else if ((Mod(actor->counter2AC, 4) == 0) && (route->speed <= 19))
        route->speed += 2;
}
