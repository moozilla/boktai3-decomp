#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_actor_position_search.h"

u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
struct ReturnResponseGroup {
    u8 unknown0[0x7C];
    s16 *coordinates;
    u8 unknown80[8];
    struct SceneActorPositionSource **source;
    struct SceneActorReactionCommand command;
    u8 unknownA0[0x1FC - 0xA0];
    void *shape;
    struct ContactPosition *destination;
};

struct ReturnResponseCoordinates {
    u8 unknown0[8];
    s16 *coordinates;
};

static inline void ClearMotionFlag(struct SceneActorRegistryResetView *actor, u32 flag)
{
    u16 *flags = &actor->flags2D4;
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
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void *ReadNavigationShape(struct ReturnResponseGroup *const *group)
{
    return (*group)->shape;
}

static inline u8 MoveTo(struct SceneActorRegistryResetView *owner,
                        struct SceneActorRegistryResetView *actor,
                        struct ReturnResponseGroup *group, struct ContactPosition *destination)
{
    void *shape = ReadNavigationShape(&group);
    struct ContactPosition *position = &actor->body.position;
    if (Nav_IsPositionInMesh(shape, position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&owner->body.motion, 8);
            return 1;
        }
    }
    ClearMotionFlag(owner, 0x200);
    return 0;
}

static inline void SetCoordinates(struct ContactPosition *out, struct ReturnResponseGroup *group)
{
    s16 *coordinates = group->coordinates;
    out->x = coordinates[0];
    out->z = coordinates[1];
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
static inline void SetResponseState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                                    u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u8 MoveToSource(struct SceneActorRegistryResetView *owner,
                              struct SceneActorRegistryResetView *actor,
                              struct ReturnResponseGroup *group,
                              struct SceneActorPositionSource **source)
{
    void *shape = group->shape;
    struct ContactPosition *position = &actor->body.position;
    struct ContactPosition *destination = &(*source)->position;
    if (Nav_IsPositionInMesh(shape, position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&owner->body.motion, 8);
            return 1;
        }
    }
    ClearMotionFlag(owner, 0x200);
    return 0;
}

static inline void SetResponseWordFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u32 *flags = &actor->flags2CC;
    *flags = mask | *flags;
}

static inline void SetResponseHalfFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 *flags = &actor->flags2D4;
    *flags = mask | *flags;
}

void SceneActorAttachment_BeginReturnResponse(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryResetView *owner = actor;
    struct ReturnResponseGroup *group = (void *)&actor->group;
    struct ContactPosition position;
    struct ContactPosition *destination;
    struct ReturnResponseCoordinates *source;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    destination = actor->state5D4;
    source = (void *)&actor->route;
    destination->x = source->coordinates[0];
    destination->z = source->coordinates[1];
    destination->y = HeightAt(destination);
    if (!MoveToSource(owner, actor, group, group->source)) {
        SetCoordinates(&position, group);
        MoveTo(owner, actor, group, &position);
        SetControl(actor, actor->entryTarget228, 1);
        SetResponseState(actor, 0, 0, 2);
    } else {
        struct SceneActorReactionCommand *command;
        SceneActorAttachment_ReplaceResponseSlot(owner, 0, 1, actor->category);
        SetResponseState(actor, 3, 1, 0);
        SetControl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 14);
        command = &group->command;
        ArmCommand(command, actor->entryTarget228, 7, 30);
        SetResponseWordFlag(actor, 0x10);
        SetResponseHalfFlag(actor, 0x1000);
        group->destination->pad = 0;
    }
}
