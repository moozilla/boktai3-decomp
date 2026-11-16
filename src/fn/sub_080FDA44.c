#include "global.h"
#include "scene_route_sample.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
struct AxePlayer {
    u8 pad[0x30];
    struct ContactPosition position;
};
void Axe_EmitMotionTrail(struct ChaseActor *actor);
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
struct AxeRoute {
    u8 pad[0xE5A];
    u16 speed;
    struct ContactPosition points[9];
    u8 index;
    u8 padEA5[0xF14 - 0xEA5];
    struct ContactPosition motion;
};
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline u32 RandomFirst(u32 bits)
{
    u32 index, value;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    index = gRandomIndex * 2;
    value = *(const u16 *)(index + RANDOM_TABLE_ADDR);
    return value & bits;
}
static inline u16 Random_FollowDeceleratingRoute(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
}
static inline u16 RandomMotion(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
static inline u32 Facing(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}
// confidence=low: follows nine stored route points and retargets around the player; action role tentative.
void Axe_FollowDeceleratingRoute(struct ChaseActor *actor)
{
    struct AxeRoute *route = actor->base.context;
    struct ContactPosition delta, target;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        u32 origin, savedOrigin;
        u32 angle;
        void *callback = actor->base.onAlternateEntry;
        u32 one = 1;
        actor->base.field2B2 = one;
        actor->base.stepDone = 0;
        actor->base.controlKind = 1;
        actor->base.target = callback;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = 0;
        route->index = 0;
        target = (*(struct AxePlayer **)gPlayers)->position;
        origin = (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
        angle = RandomFirst(255);
        SceneRoute_ProjectRandomOffset(&delta, angle, 512, origin, &savedOrigin);
        target.x += delta.x;
        target.z += delta.z;
        Delta(&delta, &actor->base.body.position, &target);
        Polar(&route->motion,
              Facing(ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR),
                     &actor->base.tracking.heading) &
                  255,
              32);
        route->speed = 1;
    }
    Delta(&delta, &actor->base.body.position, &route->points[route->index]);
    Axe_EmitMotionTrail(actor);
    if (delta.x * delta.x + delta.z * delta.z > 1024) {
        actor->base.body.direction =
            Math_Atan2(route->points[route->index].x - actor->base.body.position.x,
                       route->points[route->index].z - actor->base.body.position.z);
        Polar(&delta, actor->base.body.direction, route->speed);
        actor->base.body.motion.x += delta.x;
        actor->base.body.motion.z += delta.z;
        route->motion = actor->base.body.motion;
    } else if (++route->index > 8) {
        Delta(&route->motion, &actor->base.body.position, &route->points[8]);
        route->motion.y = 0;
        actor->base.body.position.x = route->points[8].x;
        actor->base.body.position.z = route->points[8].z;
        route->index = 0;
        target = (*(struct AxePlayer **)gPlayers)->position;
        Polar(&delta, Facing(Random_FollowDeceleratingRoute(), &actor->base.tracking.heading) & 255, 512);
        target.x += delta.x;
        target.z += delta.z;
        Delta(&delta, &actor->base.body.position, &target);
        route->speed >>= 1;
        return;
    }
    if (Mod(actor->base.counter2AC, 16) == 0 && route->speed <= 11)
        ++route->speed;
    ++actor->base.counter2AC;
}
