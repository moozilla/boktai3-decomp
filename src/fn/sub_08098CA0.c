// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
struct GhostPlayer {
    u8 pad[0x30];
    struct ContactPosition position;
};
extern struct GhostPlayer *gUnk_02000580;
void Ghost_EmitAfterimage(struct SceneActorRegistryResetView *);
void sub_08098344(void *, void *);
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
struct GhostRoute {
    u8 pad[0x566];
    u16 speed;
    struct ContactPosition points[9];
    u8 index;
    u8 pad5B1[0x5FC - 0x5B1];
    struct ContactPosition motion;
};
static inline s32 Scale(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * speed);
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
static inline u16 Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)gUnk_0203B400);
}
static inline u16 RandomMotion(void)
{
    u32 offset;
    const u16 *table;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    table = gUnk_0203B400;
    return *(const u16 *)(offset + (u32)table);
}
static inline u32 Facing(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}
// confidence=low: follows nine stored route points and retargets around the player; action role tentative.
void Ghost_FollowPlayerLoop(struct SceneActorRegistryResetView *actor)
{
    struct GhostRoute *route = actor->context;
    struct ContactPosition delta, target;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        register u32 observed asm("r4");
        u32 angle;
        void *callback = *(void **)((u8 *)actor + 0x228);
        u32 one = 1;
        actor->field2B2 = one;
        actor->field2B0 = 0;
        actor->field2B5 = 1;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        route->index = 0;
        target = gUnk_02000580->position;
        observed = 0x0403B714;
        angle = RandomFirst(255);
        Polar(&delta, angle, 512);
        asm volatile("" ::"r"(observed));
        target.x += delta.x;
        target.z += delta.z;
        Delta(&delta, &actor->position, &target);
        Polar(&route->motion, Facing(RandomMotion(), &actor->fieldE9) & 255, 32);
        sub_08098344(actor, &target);
        route->speed = 1;
    }
    Delta(&delta, &actor->position, &route->points[route->index]);
    Ghost_EmitAfterimage(actor);
    if (delta.x * delta.x + delta.z * delta.z > 1024) {
        actor->direction = Math_Atan2(route->points[route->index].x - actor->position.x,
                                      route->points[route->index].z - actor->position.z);
        Polar(&delta, actor->direction, route->speed);
        actor->motion.x += delta.x;
        actor->motion.z += delta.z;
        route->motion = actor->motion;
    } else if (++route->index > 8) {
        Delta(&route->motion, &actor->position, &route->points[8]);
        route->motion.y = 0;
        actor->position.x = route->points[8].x;
        actor->position.z = route->points[8].z;
        route->index = 0;
        target = gUnk_02000580->position;
        Polar(&delta, Facing(Random(), &actor->fieldE9) & 255, 512);
        target.x += delta.x;
        target.z += delta.z;
        Delta(&delta, &actor->position, &target);
        sub_08098344(actor, &target);
        route->speed >>= 1;
        return;
    }
    if (*(u16 *)((u8 *)actor + 0x1E6) == 0) {
        if (Mod(actor->counter2AC, 16) == 0 && route->speed <= 11)
            ++route->speed;
    } else {
        if (Mod(actor->counter2AC, 8) == 0 && route->speed <= 11)
            route->speed += 2;
    }
    ++actor->counter2AC;
}
