// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_actor_position_search.h"

#ifdef NONMATCHING
// fuzzy: 89.4%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
/* Remaining differences: pointer lifetimes, state/reset constants, and command setup. */
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
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
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u8 MoveTo(struct SceneActorRegistryResetView *actor, void *group,
                        struct ContactPosition *destination)
{
    void *shape = *((void **)(((u8 *)group) + 0x1FC));
    if (Nav_IsPositionInMesh(shape, &actor->position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, &actor->position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        }
    }
    {
        s32 mask = ~0x200;
        actor->flags2D4 &= mask;
    }
    return 0;
}

static inline u8 MoveToSource(struct SceneActorRegistryResetView *actor, void *group,
                              struct SceneActorPositionSource **source)
{
    void *shape = *((void **)(((u8 *)group) + 0x1FC));
    register struct SceneActorRegistryResetView *owner asm("r8") = actor;
    struct ContactPosition *destination = &(*source)->position;
    if (Nav_IsPositionInMesh(shape, &actor->position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, &actor->position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&owner->motion, 8);
            return 1;
        }
    }
    {
        s32 mask = ~0x200;
        owner->flags2D4 &= mask;
    }
    return 0;
}

static inline void SetCoordinates(struct ContactPosition *out, s16 *coordinates)
{
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
static inline void SetWordFlag(u32 *flags, u32 mask)
{
    *flags = mask | *flags;
}
static inline void SetHalfFlag(u16 *flags, u32 mask)
{
    *flags = mask | *flags;
}
inline static void SetState_BeginReturnResponse(struct SceneActorRegistryResetView *actor,
                                                u32 state, u32 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

inline static u8 MoveToSource_BeginReturnResponse(struct SceneActorRegistryResetView *actor,
                                                  void *group,
                                                  struct SceneActorPositionSource **source)
{
    void *shape = *((void **)(((u8 *)group) + 0x1FC));
    register struct SceneActorRegistryResetView *owner asm("r8") = actor;
    struct ContactPosition *destination = &(*source)->position;
    if (Nav_IsPositionInMesh(shape, &actor->position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, &actor->position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&owner->motion, 8);
            return 1;
        }
    }
    {
        u8 mask = ~0x200;
        owner->flags2D4 &= mask;
    }
    return 0;
}

inline static void SetWordFlag_BeginReturnResponse(u32 *flags, u32 mask)
{
    do {
        *flags = mask | (*flags);
    } while (0);
}

void SceneActorAttachment_BeginReturnResponse(struct SceneActorRegistryResetView *actor)
{
    register void *group asm("sb") = &actor->group;
    struct ContactPosition position;
    struct ContactPosition *destination;
    void **source;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    destination = actor->state5D4;
    source = (void **)(((u8 *)actor) + 0x448);
    destination->x = (*((s16 **)(((u8 *)source) + 8)))[0];
    destination->z = (*((s16 **)(((u8 *)source) + 8)))[1];
    destination->y = HeightAt(destination);
    if (!MoveToSource_BeginReturnResponse(
            actor, group, *((struct SceneActorPositionSource ***)(((u8 *)group) + 0x88)))) {
        s16 *coordinates = *((s16 **)(((u8 *)group) + 0x7C));
        SetCoordinates(&position, coordinates);
        MoveTo(actor, group, &position);
        SetControl(actor, actor->entryTarget228, 1);
        SetState_BeginReturnResponse(actor, 0, 0, 2);
    } else {
        struct SceneActorReactionCommand *command;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->category);
        SetState_BeginReturnResponse(actor, 3, 1, 0);
        SetControl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 14);
        command = (void *)(((u8 *)group) + 0x8C);
        ArmCommand(command, actor->entryTarget228, 7, 30);
        SetWordFlag_BeginReturnResponse(&actor->flags2CC, 0x10);
        SetHalfFlag(&actor->flags2D4, 0x1000);
        ((struct ContactPosition *)actor->state5D4)->pad = 0;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorAttachment_BeginReturnResponse);
#endif
