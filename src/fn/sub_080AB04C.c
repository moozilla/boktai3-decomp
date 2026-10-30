#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
static inline u32 WordFlag(struct SceneActorRegistryResetView *p, u32 mask)
{
    return mask & p->flags2D0;
}
static inline u8 IsInactive(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if ((actor->flags2D4 & mask) || WordFlag(actor, 0x20000) || actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
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

void TentacleActor_SelectWanderRoute(void *);
s32 Math_Atan2(s32, s32);
static inline void Add(struct ContactPosition *out, const struct ContactPosition *a,
                       const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
void TentacleActor_ResolveBlockedMovement(struct SceneActorRegistryResetView *actor)
{
    u8 *trail;
    struct ContactPosition target;
    s32 index, height;
    u32 attributes;
    if (IsInactive(actor))
        return;
    trail = actor->context;
    Add(&target, &actor->body.position, &actor->secondaryMotion);
    index = SceneActorTerrain_GetCellIndex(&target);
    attributes = ((u16 *)SceneActorTerrain_GetCell(index))[1];
    height = Height(index, &target);
    if (actor->groundHeight2BE < height || (attributes & 0x43)) {
        actor->secondaryMotion.x = 0;
        actor->secondaryMotion.z = 0;
    }
    {
        u32 mask = 0x2000;
        u32 *flags = &actor->flags2CC;
        *flags = *flags | mask;
    }
    if (**(u8 **)((u8 *)actor + 0x6c) & 1) {
        u8 *count = trail + 0x879;
        ++*count;
        if (*count > 30) {
            s32 difference;
            trail[0x8e9] = (trail[0x8e9] + 1) & 3;
            TentacleActor_SelectWanderRoute(actor);
            Delta(&target, &actor->body.position, (void *)(trail + 0x904));
            *((u8 *)actor + 0x3cd) = Math_Atan2(target.x, target.z);
            difference = *((u8 *)actor + 0x3cd) - actor->body.direction;
            if (difference > 128)
                difference += -256;
            else if (difference < -128)
                difference += 256;
            if (difference < 0)
                difference = -difference;
            if (difference > 32) {
                u32 behavior = 1;
                actor->state2A8 = 0;
                actor->phase2A9 = 0;
                actor->behavior2AA = behavior;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }
        }
    } else
        trail[0x879] = 0;
}
