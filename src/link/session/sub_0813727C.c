#include "scene/actor/scene_actor_control_begin.h"
#include "scene/terrain/terrain_sampling.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene/motion/scene_actor_terrain.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_chase_shared.h"

#define gPlayerSlotFlags gUnk_02000560
extern u32 gUnk_02000560;
s32 Callback_InvokeActor(void *, void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorReaction_RestoreAfterDelay(void *);

static inline u32 ActorFlag(struct ChaseActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? actor->base.motionFlags & mask : actor->base.statusFlags & mask;
}

static inline u8 IsUnavailable(struct ChaseActor *actor)
{
    if (actor && (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) ||
                  actor->base.health <= 0))
        return 1;
    return 0;
}

static inline u32 TestSlot(s32 i)
{
    u32 mask = 8;
    return gPlayerSlotStatus[i] & mask;
}




struct FlagHalfword {
    s16 bits;
};
static inline void ClearHalfFlags(u16 *flags, u32 mask)
{
    struct FlagHalfword *field = (void *)flags;
    field->bits = mask & field->bits;
}

static inline u8 MoveTo(struct ChaseActor *actor, void *shape, struct ContactPosition *position,
                        void *destination, u16 *const *motionFlags, u32 blocked)
{
    u16 *flags = *motionFlags;
    s32 result;

    if (!Nav_IsPositionInMesh(shape, position, destination)) {
        *flags &= ~blocked;
        return 0;
    }
    result = Terrain_ApplyBlocked(shape, position, destination);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->base.body.motion, 8);
        return 1;
    }
    *flags &= ~blocked;
    return 0;
}

static inline u32 ActorFlags(struct ChaseActor *actor, u32 mask)
{
    u32 bit = mask;
    u32 *flags = &actor->base.behaviorFlags;
    return *flags & bit;
}
static inline void ResetControl(struct ChaseActor *actor, u32 enabled, u16 *flags)
{
    struct SceneActorRegistryControl *control = &actor->group.common.control;
    if (enabled) {
        SceneActorChase_AndWordFlags(&actor->base.statusFlags, ~0x1000000);
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

struct ReactionControlPair {
    struct SceneActorChaseSavedControl saved;
    struct SceneActorTimedRestoration timed;
};

static inline void SaveControl(struct ReactionControlPair *record, void *callback, u8 behavior)
{
    record->timed.threshold = 30;
    record->timed.control = callback;
    record->timed.state = 0;
    record->timed.phase = 0;
    record->timed.behavior = behavior;
}
static inline void SetFlags(struct ChaseActor *actor, u32 mask)
{
    u32 bit = mask;
    u32 *flags = &actor->base.behaviorFlags;
    *flags |= bit;
}
u32 sub_0813727C(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group;
    struct SceneActorChaseTarget *target;
    s32 slot;
    u16 flags;
    u16 *flagsp;
    struct ContactPosition *dest;
    if (IsUnavailable(actor))
        return 0;
    target = actor->base.tracking.target;
    if (!target)
        return 0;
    slot = SceneActorChase_PlayerSlot(target);
    if (slot < 0)
        return 0;
    if (!TestSlot(slot))
        return 0;
    {
        u32 busy = 0xA027;
        u16 *p = &actor->base.motionFlags;
        u32 t;
        flags = *p;
        t = busy & flags;
        flagsp = p;
        if (t)
            return 0;
    }
    if (*(u32 *)&actor->base.secondaryMotion)
        return 0;
    if (actor->base.secondaryMotion.z)
        return 0;
    group = &actor->group;
    {
        s32 bits = ~0x20;
        bits &= *flagsp;
        ((struct FlagHalfword *)flagsp)->bits = bits;
    }
    actor->base.field2C2 = 0x40;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    actor->base.tracking.target = 0;
    SceneActorTerrain_EnterState(actor, 3, 1, 0);
    SceneActorControl_Begin(actor, SceneActorReaction_RestoreAfterDelay, 14, 0);
    SetFlags(actor, 0x10);
    SaveControl((void *)&group->saved, actor->base.onAlternateEntry, 7);
    ResetControl(actor, ActorFlags(actor, 0x200000), flagsp);
    ProximityVolume_TranslateBounds(actor->group.route, &actor->base.body.position);
    dest = actor->group.destination;
    {
        struct SceneActorRegistryRoute *link = &group->homeRoute;
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = CellHeight(Terrain_GetCellIndex(dest), dest);
    }
    group->destination->pad = 1;
    if (!MoveTo(actor, group->route, &actor->base.body.position, group->destination, &flagsp,
                0x200)) {
        Callback_InvokeActor(actor, group->fallback);
    } else {
        u16 *resetFlags;
        group->common.control.counter = 0;
        group->common.control.field14 = 0;
        actor->base.field2C2 = 0x40;
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
            struct SceneActorReactionMotion *motion = (void *)&actor->base.tracking.target;
            motion->flags &= mask;
        }
        ResetControl(actor, ActorFlags(actor, 0x200000), resetFlags);
        ClearHalfFlags(resetFlags, ~0x400);
        actor->group.turn.turnsRemaining = 0xFF;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    }
    return 1;
}
