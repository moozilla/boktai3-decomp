#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
void sub_0807E334(void *);

struct ApproachRoutePoint {
    s16 x, z;
};
struct ApproachRouteLink {
    u8 unknown0[8];
    struct ApproachRoutePoint *point;
};
struct ApproachSaved {
    void *target;
    u8 state, phase, behavior, kind;
};
struct ApproachGroup {
    u8 unknown0[0x40];
    u16 flags414, field416, field418;
    u8 unknown46[2];
    void *entryControl41C;
    u8 unknown4C[0x28];
    struct ApproachRouteLink link;
    u8 unknown80[0xC];
    struct ApproachSaved saved;
    u8 unknown94[0x168];
    void *routeObject;
    struct ContactPosition *destination;
    u8 direction, savedDirection, timer, unknown207;
    u8 unknown208[0x14];
    void *control;
    u8 unknown220[4];
    void *failure;
};

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}
static inline u8 Chase(struct SceneActorRegistryResetView *actor, struct ApproachGroup *group,
                       u32 bits)
{
    u8 *target = *(u8 **)&actor->tracking.target;
    void *route = group->routeObject;
    struct ContactPosition *position = &actor->body.position;
    struct ContactPosition *destination = (void *)(target + 12);
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        {
            actor->flags2D4 &= ~bits;
        }
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        } else {
            {
                actor->flags2D4 &= ~bits;
            }
            return 0;
        }
    }
}

struct SkeletonRouteContext {
    u8 unknown0[0x6D0];
    u8 active;
};
static inline u16 MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 value = actor->flags2D4;
    value &= mask;
    return value;
}
static inline void OrFlags(u16 *flags, u32 mask)
{
    u32 value = mask;
    value |= *flags;
    *flags = value;
}

// confidence=low: category-independent callback restores route target and resumes pursuit
void Skeleton_RestorePursuitRoute(struct SceneActorRegistryResetView *actor)
{
    struct SkeletonRouteContext *context = actor->context;
    struct ApproachGroup *group = (void *)&actor->group;
    struct ContactPosition *destination;
    if (MotionFlags(actor, 0x100) || context->active)
        return;
    if (MotionFlags(actor, 0x80)) {
        SetState(actor, 0, 0, 12);
        SetControl(actor, group->entryControl41C, 0x1A);
        if (MotionFlags(actor, 0x80)) {
            group->field418 += 300;
            if (group->field418 > group->field416)
                group->field418 = group->field416;
        }
        OrFlags(&group->flags414, 4);
    } else {
        struct SceneActorReactionCommand *command;
        SetState(actor, 3, 1, 0);
        SetControl(actor, SceneActorReaction_RestoreAfterDelay, 0xE);
        {
            u32 mask = 0x10;
            actor->flags2CC |= mask;
        }
        actor->field2C2 = 0x20;
        command = (void *)((u8 *)actor + 0x460);
        {
            void *payload = actor->entryTarget228;
            u32 kind = 7;
            command->duration = 60;
            command->payload = payload;
            command->first = 0;
            command->second = 0;
            command->kind = kind;
        }
    }
    destination = group->destination;
    {
        struct ApproachRouteLink *link = &group->link;
        destination->x = link->point->x;
        destination->z = link->point->z;
    }
    destination->y = CellHeight(destination, CellIndex(destination));
    {
        u32 routeMask = 0x200;
        (*(struct ContactPosition **)((u8 *)group + routeMask))->pad = 0;
        if (!Chase(actor, group, routeMask))
            Callback_InvokeActor(actor, group->failure);
    }
}
