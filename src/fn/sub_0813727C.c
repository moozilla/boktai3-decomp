#define gPlayerSlotFlags gUnk_02000560
#include "global.h"
#include "scene_actor_terrain.h"
#include "scene_actor_reset.h"
#ifdef NONMATCHING
// fuzzy: 76.4%
// Open: mostly register allocation (ROM: actor r5, group sl, flags2D4 pointer r8,
// position spilled to sp); also a few reordered loads and spill-slot order.

extern u32 gUnk_02000560;
s32 Callback_InvokeActor(void *, void *);
void SceneActorAnimation_SelectDirectionalEntry(void *, u32);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
u32 Nav_IsPositionInMesh(void *, struct ContactPosition *, void *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, void *);
void SceneActorReaction_RestoreAfterDelay(void *);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

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
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

static inline u32 CellHeight(u32 cell, struct ContactPosition *position)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
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

static inline void ClearWordFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}

static inline void ClearHalfFlags(u16 *flags, u32 mask)
{
    *flags &= mask;
}

struct RoutePoint {
    s16 x, z;
};
struct RouteLink {
    u8 unknown0[8];
    struct RoutePoint *point;
};

#define GROUP_FIELD(g, off, T) (*(T *)((u8 *)(g) + (off)))

static inline u8 MoveTo(struct SceneActorRegistryResetView *actor, u8 *group,
                        struct ContactPosition *position, u16 *flags, u32 blocked)
{
    void *shape = GROUP_FIELD(group, 0x1fc, void *);
    void *destination = GROUP_FIELD(group, blocked, void *);
    s32 result;
    if (!Nav_IsPositionInMesh(shape, position, destination)) {
        *flags &= ~blocked;
        return 0;
    }
    result = Terrain_ApplyBlocked(shape, position, destination);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->motion, 8);
        return 1;
    }
    *flags &= ~blocked;
    return 0;
}

static inline void ResetControl(struct SceneActorRegistryResetView *actor, u32 *flags2CC,
                                u16 *flags, struct SceneActorRegistryControl *control)
{
    u32 mask = 0x200000;
    if (*flags2CC & mask) {
        ClearWordFlags(&actor->flags2D0, ~0x1000000);
        ClearHalfFlags(flags, ~0x200);
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
}

// confidence=low: scene actor starts reacting to its link-player target: drops the target, enters state 3 with RestoreAfterDelay, then walks to the route point.
u32 sub_0813727C(struct SceneActorRegistryResetView *actor)
{
    u8 *group;
    u32 *targetp;
    struct SceneActorReactionMotion *motion;
    u8 *target;
    s32 slot;
    u16 flags;
    u16 *flagsp;
    u32 *flags2CC;
    struct ContactPosition *position;
    struct ContactPosition *dest;
    struct SceneActorRegistryControl *control;
    u32 bit;

    if (IsUnavailable(actor))
        return 0;
    motion = (struct SceneActorReactionMotion *)&actor->fieldDC;
    targetp = &motion->fieldDC;
    target = (u8 *)*targetp;
    if (!target)
        return 0;
    slot = PlayerSlot(target);
    if (slot < 0)
        return 0;
    if (!TestSlot(slot))
        return 0;
    {
        u32 busy = 0xA027;
        flagsp = &actor->flags2D4;
        flags = *flagsp;
        if (busy & flags)
            return 0;
    }
    if (FIELD(actor, 0x2EC, u32))
        return 0;
    if (FIELD(actor, 0x2F0, s16))
        return 0;
    group = (u8 *)&actor->group;
    ClearHalfFlags(flagsp, ~0x20);
    actor->field2C2 = 0x40;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    *targetp = 0;
    SetState(actor, 3, 1, 0);
    SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    flags2CC = &actor->flags2CC;
    *flags2CC |= 0x10;
    {
        u8 *saved = group + 0x8C;
        void *callback = actor->entryTarget228;
        FIELD(saved, 0xC, u32) = 30;
        FIELD(saved, 0x10, void *) = callback;
        saved[8] = 0;
        saved[9] = 0;
        saved[10] = 7;
    }
    control = &actor->group.control;
    ResetControl(actor, flags2CC, flagsp, control);
    ProximityVolume_TranslateBounds(actor->routeObject, position = &actor->position);
    dest = actor->state5D4;
    {
        struct RouteLink *link = (void *)&actor->route;
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = CellHeight(CellIndex(dest), dest);
    }
    bit = 0x200;
    GROUP_FIELD(group, bit, struct ContactPosition *)->pad = 1;
    if (!MoveTo(actor, group, &actor->position, flagsp, bit)) {
        Callback_InvokeActor(actor, GROUP_FIELD(group, 0x224, void *));
    } else {
        GROUP_FIELD(group, 0x64, u32) = 0;
        GROUP_FIELD(group, 0x60, u16) = 0;
        actor->field2C2 = 0x40;
        FIELD(actor, 0x414, u16) = 0;
        FIELD(actor, 0x418, u16) = 0;
        ClearHalfFlags(flagsp, ~0x380);
        {
            s32 mask = ~0x18;
            motion->flags &= mask;
        }
        ResetControl(actor, flags2CC, flagsp, control);
        ClearHalfFlags(flagsp, ~0x400);
        actor->field5DA = 0xFF;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
        SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0813727C);
#endif
