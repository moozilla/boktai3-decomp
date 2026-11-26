#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_reaction.h"
#include "engine/engine.h"
#include "scene/motion/scene_actor_position_search.h"
#include "scene/motion/scene_actor_chase_shared.h"

u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
struct ReturnResponseGroup {
    u8 unknown0[0x7C];
    struct SceneActorChaseRoutePoint *point;
    u8 unknown80[8];
    struct SceneActorPositionSource **source;
    struct SceneActorReactionCommand command;
    u8 unknownA0[0x1FC - 0xA0];
    void *shape;
    struct ContactPosition *destination;
};

struct ReturnResponseCoordinates {
    u8 unknown0[8];
    struct SceneActorChaseRoutePoint *point;
};

static inline void ClearMotionFlag(struct ChaseActor *actor, u32 flag)
{
    u16 *flags = &actor->base.motionFlags;
    u32 mask = ~flag;
    *flags &= mask;
}

static inline u32 CellAt(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *CellData(u32 cell)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        return data + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}
static inline s32 HeightAt(struct ContactPosition *position)
{
    u8 *data = CellData(CellAt(position));
    s32 shape = *data >> 4;
    s32 height = (*data & 15) << 8;
    switch (shape) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
static inline void *ReadNavigationShape(struct ReturnResponseGroup *const *group)
{
    return (*group)->shape;
}

static inline u8 MoveTo(struct ChaseActor *owner, struct ChaseActor *actor,
                        struct ReturnResponseGroup *group, struct ContactPosition *destination)
{
    void *shape = ReadNavigationShape(&group);
    struct ContactPosition *position = &actor->base.body.position;
    if (Nav_IsPositionInMesh(shape, position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&owner->base.body.motion, 8);
            return 1;
        }
    }
    ClearMotionFlag(owner, 0x200);
    return 0;
}

static inline void SetCoordinates(struct ContactPosition *out, struct ReturnResponseGroup *group)
{
    struct SceneActorChaseRoutePoint *point = group->point;
    out->x = point->x;
    out->z = point->z;
    out->y = HeightAt(out);
}
static inline void ArmCommand(struct SceneActorReactionCommand *command, void *payload, u32 kind,
                              u32 duration)
{
    command->duration = duration;
    command->payload = payload;
    command->first = 0;
    command->second = 0;
    command->kind = kind;
}

static inline u8 MoveToSource(struct ChaseActor *owner, struct ChaseActor *actor,
                              struct ReturnResponseGroup *group,
                              struct SceneActorPositionSource **source)
{
    void *shape = group->shape;
    struct ContactPosition *position = &actor->base.body.position;
    struct ContactPosition *destination = &(*source)->position;
    if (Nav_IsPositionInMesh(shape, position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&owner->base.body.motion, 8);
            return 1;
        }
    }
    ClearMotionFlag(owner, 0x200);
    return 0;
}

void SceneActorAttachment_BeginReturnResponse(struct ChaseActor *actor)
{
    struct ChaseActor *owner = actor;
    struct ReturnResponseGroup *group = (void *)&actor->group.common;
    struct ContactPosition position;
    struct ContactPosition *destination;
    struct ReturnResponseCoordinates *source;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    destination = actor->group.destination;
    source = (void *)&actor->group.homeRoute;
    destination->x = source->point->x;
    destination->z = source->point->z;
    destination->y = HeightAt(destination);
    if (!MoveToSource(owner, actor, group, group->source)) {
        SetCoordinates(&position, group);
        MoveTo(owner, actor, group, &position);
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorTerrain_EnterState(actor, 0, 0, 2);
    } else {
        struct SceneActorReactionCommand *command;
        SceneActorAttachment_ReplaceResponseSlot(owner, 0, 1, actor->base.category);
        SceneActorTerrain_EnterState(actor, 3, 1, 0);
        SceneActorTerrain_InstallControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        command = &group->command;
        ArmCommand(command, actor->base.onAlternateEntry, 7, 30);
        SceneActorChase_SetBehaviorFlags(actor, 0x10);
        SceneActorChase_SetMotionFlags(actor, 0x1000);
        group->destination->pad = 0;
    }
}
