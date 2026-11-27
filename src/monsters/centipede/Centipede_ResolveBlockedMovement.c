#include "monsters/centipede/centipede_body.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_availability.h"
#include "scene/motion/scene_actor_terrain.h"
#include "scene/motion/scene_actor_chase.h"
static inline s32 Height(s32 cell, struct ContactPosition *p)
{
    u32 attr = *SceneActorTerrain_GetCell(cell);
    u32 kind = attr >> 4;
    u32 mask = 15;
    s32 height;
    mask &= attr;
    height = mask << 8;
    switch (kind) {
        case 1:
            height -= ((u8 *)p)[4];
            break;
        case 2:
            height -= ((u8 *)p)[0];
            break;
    }
    return height;
}

void Centipede_SelectWanderRoute(void *);
s32 Math_Atan2(s32, s32);
static inline void Add(struct ContactPosition *out, const struct ContactPosition *a,
                       const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
void Centipede_ResolveBlockedMovement(struct ChaseActor *actor)
{
    struct TentacleBody *trail;
    struct ContactPosition target;
    s32 index, height;
    u32 attributes;
    if (SceneActor_ActionsBlocked(actor))
        return;
    trail = actor->base.context;
    Add(&target, &actor->base.body.position, &actor->base.secondaryMotion);
    index = SceneActorTerrain_GetCellIndex(&target);
    attributes = ((u16 *)SceneActorTerrain_GetCell(index))[1];
    height = Height(index, &target);
    if (actor->base.groundHeight < height || (attributes & 0x43)) {
        actor->base.secondaryMotion.x = 0;
        actor->base.secondaryMotion.z = 0;
    }
    {
        u32 mask = 0x2000;
        u32 *flags = &actor->base.behaviorFlags;
        *flags = *flags | mask;
    }
    if (**(u8 **)((u8 *)actor + 0x6c) & 1) {
        u8 *count = &trail->blockedFrameCount;
        ++*count;
        if (*count > 30) {
            s32 difference;
            trail->wanderDirection = (trail->wanderDirection + 1) & 3;
            Centipede_SelectWanderRoute(actor);
            Chase_Delta(&target, &actor->base.body.position, &trail->waypoint);
            actor->base.nextDirection = Math_Atan2(target.x, target.z);
            difference = actor->base.nextDirection - actor->base.body.direction;
            if (difference > 128)
                difference += -256;
            else if (difference < -128)
                difference += 256;
            if (difference < 0)
                difference = -difference;
            if (difference > 32) {
                u32 behavior = 1;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
        }
    } else
        trail->blockedFrameCount = 0;
}
