// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_terrain.h"
#include "scene_actor_reset.h"

#define gPlayerSlotFlags gUnk_02000560
extern u32 gUnk_02000560;
s32 Callback_InvokeActor(void *, void *);
void SceneActorAnimation_SelectDirectionalEntry(void *, u32);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
u32 Nav_IsPositionInMesh(void *, struct ContactPosition *, void *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, void *);
void SceneActorReaction_RestoreAfterDelay(void *);

static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}

static inline u8 IsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}

static inline s32 PlayerSlot(u8 *target)
{
    if (target[8] <= 3)
        return target[8];
    return -1;
}

static inline u32 TestSlot(s32 i)
{
    u32 mask = 8;
    u32 *flags = &gPlayerSlotFlags;
    return flags[i] & mask;
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

static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline void ClearWordFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}

struct FlagHalfword {
    s16 bits;
};
static inline void ClearHalfFlags(u16 *flags, u32 mask)
{
    struct FlagHalfword *field = (void *)flags;
    field->bits = mask & field->bits;
}

struct RoutePoint {
    s16 x, z;
};
struct RouteLink {
    u8 unknown0[8];
    struct RoutePoint *point;
};

struct ReactionControl {
    u8 unknown0[8];
    u16 timer;
    u8 mode, unknownB[9];
    u16 recoveryTimer, unknown16;
    u32 counter;
};
struct ReactionSavedControl {
    u8 unknown0[8];
    u8 state, phase, behavior, unknownB;
    u32 counter;
    void *callback;
};
struct ReactionGroup {
    u8 unknown0[0x40];
    u16 flags414, unknown42, flags418;
    u8 unknown46[2];
    void *entryControl;
    struct ReactionControl control;
    u8 unknown68[0xC];
    struct RouteLink route;
    u8 unknown80[0xC];
    struct ReactionSavedControl saved;
    u8 unknownA0[0x15C];
    void *shape;
    struct ContactPosition *destination;
    u8 unknown204[0x20];
    void *failureCallback;
};

static inline u8 MoveTo(struct SceneActorRegistryResetView *actor, void *shape,
                        struct ContactPosition *position, void *destination, u16 *flags,
                        u32 blocked)
{
    s32 result;
    if (!Nav_IsPositionInMesh(shape, position, destination)) {
        *flags &= ~blocked;
        return 0;
    }
    result = Terrain_ApplyBlocked(shape, position, destination);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->body.motion, 8);
        return 1;
    }
    *flags &= ~blocked;
    return 0;
}

static inline u32 ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u32 bit = mask;
    u32 *flags = &actor->flags2CC;
    return *flags & bit;
}
static inline void ResetControl(struct SceneActorRegistryResetView *actor, u32 enabled, u16 *flags)
{
    struct SceneActorRegistryControl *control = &actor->group.common.control;
    if (enabled) {
        ClearWordFlags(&actor->flags2D0, ~0x1000000);
        ClearHalfFlags(flags, ~0x200);
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
}

static inline u32 CellHeight(u32 cell, struct ContactPosition *position)
{
    u32 shape;
    u32 height;
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

static inline void SaveControl(void *record, void *callback, u8 behavior)
{
    struct ReactionSavedControl *saved = record;
    saved->counter = 30;
    saved->callback = callback;
    saved->state = 0;
    saved->phase = 0;
    saved->behavior = behavior;
}
static inline void SetFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u32 bit = mask;
    u32 *flags = &actor->flags2CC;
    *flags |= bit;
}
u32 sub_0813727C(struct SceneActorRegistryResetView *actor)
{
    struct ReactionGroup *group;
    u8 *target;
    s32 slot;
    u16 flags;
    u16 *flagsp;
    struct ContactPosition *dest;
    u32 bit;
    if (IsUnavailable(actor))
        return 0;
    target = (void *)actor->tracking.target;
    if (!target)
        return 0;
    slot = PlayerSlot(target);
    if (slot < 0)
        return 0;
    if (!TestSlot(slot))
        return 0;
    {
        u32 busy = 0xA027;
        u16 *p = &actor->flags2D4;
        u32 t;
        flags = *p;
        t = busy & flags;
        flagsp = p;
        if (t)
            return 0;
    }
    if (*(u32 *)&actor->secondaryMotion)
        return 0;
    if (actor->secondaryMotion.z)
        return 0;
    group = (void *)&actor->group.common;
    {
        s32 bits = ~0x20;
        bits &= *flagsp;
        ((struct FlagHalfword *)flagsp)->bits = bits;
    }
    actor->field2C2 = 0x40;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    actor->tracking.target = 0;
    SetState(actor, 3, 1, 0);
    SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SetFlags(actor, 0x10);
    SaveControl(&group->saved, actor->entryTarget228, 7);
    ResetControl(actor, ActorFlags(actor, 0x200000), flagsp);
    ProximityVolume_TranslateBounds(actor->group.route, &actor->body.position);
    dest = actor->group.destination;
    {
        struct RouteLink *link = &group->route;
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = CellHeight(CellIndex(dest), dest);
    }
    bit = 0x200;
    group->destination->pad = 1;
    if (!MoveTo(actor, group->shape, &actor->body.position, group->destination, flagsp, bit)) {
        Callback_InvokeActor(actor, group->failureCallback);
    } else {
        register u16 *resetFlags asm("r3");
        group->control.counter = 0;
        group->control.recoveryTimer = 0;
        actor->field2C2 = 0x40;
        actor->group.common.gate.flags = 0;
        actor->group.common.gate.counter = 0;
        resetFlags = flagsp;
        {
            u32 mask = ~0x380;
            ((struct FlagHalfword *)resetFlags)->bits =
                mask & ((struct FlagHalfword *)resetFlags)->bits;
        }
        {
            s32 mask = ~0x18;
            struct SceneActorReactionMotion *motion = (void *)&actor->tracking.target;
            motion->flags &= mask;
        }
        ResetControl(actor, ActorFlags(actor, 0x200000), resetFlags);
        ClearHalfFlags(flagsp, ~0x400);
        actor->group.turn.turnsRemaining = 0xFF;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    }
    return 1;
}
