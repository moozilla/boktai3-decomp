#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
// confidence=low: neighboring Skeleton controls establish provisional role
#ifdef NONMATCHING
// fuzzy: 92.8%
// Remaining differences: recovery stack slots, turn-state address lifetimes, and flag-update ordering.
void sub_0807E334(void *);

struct ApproachRoutePoint {
    s16 x, z;
};
struct ApproachRouteLink {
    u8 unknown0[8];
    struct ApproachRoutePoint *point;
};
struct ApproachPlayerCell {
    u8 unknown0[6];
    u16 cell;
};
struct ApproachPlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0x1C];
    struct ApproachPlayerCell *cell;
};
extern struct ApproachPlayer *gPlayers[4];

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
static inline u8 IsScenePaused(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}
static inline u8 TestFlag16(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline s32 PlayerSlot(u8 *target)
{
    u32 index = target[8];
    s32 slot = -1;
    if (index <= 3)
        slot = index;
    return slot;
}
static inline u8 MoveTo2(struct SceneActorRegistryResetView *actor, void *route,
                         struct ContactPosition *position, struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        {
            u16 *flags = &actor->flags2D4;
            u32 mask = ~0x200;
            u32 value = *flags;
            mask &= value;
            *flags = mask;
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
                u16 *flags = &actor->flags2D4;
                u32 mask = ~0x200;
                u32 value = *flags;
                mask &= value;
                *flags = mask;
            }
            return 0;
        }
    }
}
static inline u8 PickDirection(struct SceneActorRegistryResetView *actor)
{
    register u32 current asm("r5") = actor->body.direction;
    s32 i;
    u32 direction;
    const u16 *table = gRandomTable;
    i = 0;
    do {
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        direction = (table[gRandomIndex] & 3) << 6;
        if (direction != current)
            goto chosen;
    } while (++i <= 3);
    direction = current;
chosen:
    return direction;
}
static inline void Resume(struct SceneActorRegistryResetView *raw, u8 direction)
{
    struct SceneActorRegistryResetView *actor = raw;
    struct ApproachGroup *group = (void *)&raw->group;
    u32 mask = 0x400;
    if (!(raw->flags2D4 & mask) && !TestFlag16(&group->flags414, 4)) {
        group->direction = PickDirection(actor);
        {
            struct ApproachSaved *saved = &group->saved;
            saved->target = actor->target;
            saved->kind = actor->field2B5;
            saved->state = actor->state2A8;
            saved->phase = actor->phase2A9;
            saved->behavior = actor->behavior2AA;
        }
        group->savedDirection = direction;
        SetControl(actor, group->control, 0x1B);
        SetState(actor, 3, 1, 0);
        group->timer = 6;
    }
    sub_0807E334(raw);
}
static inline u16 MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 ReturnHome(void *p)
{
    struct SceneActorRegistryResetView *actor = p;
    struct ApproachGroup *group = (void *)&actor->group;
    struct ContactPosition *destination;
    u8 **target = (u8 **)&actor->tracking.target;
    u32 cell;
    s32 slot;
    u32 mask;
    u16 test;
    destination = actor->state5D4;
    destination->x = ((struct ApproachRouteLink *)&actor->route)->point->x;
    destination->z = ((struct ApproachRouteLink *)&actor->route)->point->z;
    destination->y = CellHeight(destination, CellIndex(destination));
    if (IsScenePaused()) {
        if (!*target)
            goto fail;
        {
            u32 index = (*target)[8];
            slot = -1;
            if (index <= 3)
                slot = index;
        }
        if (slot < 0)
            goto fail;
        cell = gPlayers[slot]->cell->cell;
    } else {
        cell = CellIndex((struct ContactPosition *)(*target + 12));
        slot = 0;
    }
    if (cell == actor->terrainCell || (gPlayers[slot]->flags & (mask = 0x20000))) {
        mask = 0x400;
        test = MotionFlags(actor, mask);
        if (test)
            return 0;
        {
            struct GroupBits {
                s16 bits : 16;
            };
            ((struct GroupBits *)&group->flags414)->bits &= ~4;
        }
        mask = 0x80;
        test = MotionFlags(actor, mask);
        if (test) {
            {
                void *control = group->entryControl41C;
                SetControl(actor, control, 0x1A);
            }
            SetState(actor, 0, 0, 12);
        } else {
            {
                void *control = actor->entryTarget228;
                SetControl(actor, control, 1);
            }
            SetState(actor, 0, 0, 5);
            destination = group->destination;
            {
                struct ApproachRouteLink *link = &group->link;
                destination->x = link->point->x;
                destination->z = link->point->z;
            }

            destination->y = CellHeight(destination, CellIndex(destination));
            group->destination->y = CellHeight(group->destination, CellIndex(group->destination));
            group->destination->pad = 0;
        }
        Resume(p, actor->body.direction);
        actor->field2C2 = 0x20;
        return 0;
    } else {
        struct ContactPosition center;
        SceneActorTerrain_GetCenter(cell, &center);
        if (!MoveTo2(p, group->routeObject, &actor->body.position, &center)) {
        fail:
            Callback_InvokeActor(actor, group->failure);
            return 0;
        }
        return 1;
    }
}

struct SkeletonSelectionContext {
    u8 unknown0[0x703];
    u8 attempts;
    u8 unknown704[4];
    u32 nearDistance, farDistance;
};
extern const u8 gUnk_0824E250[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
u32 Skeleton_TrySelectRetreatCell(struct SceneActorRegistryResetView *, u32);
// confidence=low: selects retreat or route recovery by context distance thresholds
void Skeleton_SelectDistanceReaction(struct SceneActorRegistryResetView *actor)
{
    struct SkeletonSelectionContext *context = actor->context;
    struct ApproachGroup *group = (void *)&actor->group;
    SceneActorReaction_ResetResponseState((u8 *)actor + 0x460, gUnk_0824E250, 0x4F5);
    context->attempts++;
    if (actor->tracking.distance < context->nearDistance) {
        {
            u8 *direction = &actor->body.direction;
            s32 prior = *direction;
            *direction = actor->tracking.heading + 0x80;
            if (!(u8)Skeleton_TrySelectRetreatCell(actor, prior))
                return;
        }
        {
            SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SetState(actor, 0, 0, 15);
        }
    } else if (actor->tracking.distance > context->farDistance) {
        if (!actor->tracking.target)
            actor->tracking.target = (u32)((u8 *)gPlayers[0] + 0x24);
        if (ReturnHome(actor)) {
            SetControl(actor, group->entryControl41C, 0x1A);
            SetState(actor, 0, 0, 12);
        }
    } else {
        if (!actor->tracking.target)
            actor->tracking.target = (u32)((u8 *)gPlayers[0] + 0x24);
        if (ReturnHome(actor)) {
            SetControl(actor, group->entryControl41C, 0x1A);
            SetState(actor, 0, 0, 12);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Skeleton_SelectDistanceReaction);
#endif
