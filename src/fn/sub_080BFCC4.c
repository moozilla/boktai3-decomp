// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fuzzy: 95.5%
// Skeleton (tentative): delayed recovery and route-home response.
// Remaining differences: random-turn allocation, PRE spill-slot order, and the
// destination-offset register in MoveToDestination.
u8 sub_0812F3CC(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void sub_0807E334(void *);
void SceneActorControl_OnIdle(void);

struct SkelRoutePoint {
    s16 x, z;
};
struct SkelRouteLink {
    u8 unknown0[8];
    struct SkelRoutePoint *point;
};
struct SkelPlayerCell {
    u8 unknown0[6];
    u16 cell;
};
struct SkelPlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0x1C];
    struct SkelPlayerCell *cell;
};
extern struct SkelPlayer *gPlayers[4];

struct SkelSaved {
    void *target;
    u8 state, phase, behavior, kind;
    u8 restoreState, restorePhase, restoreBehavior, unknownB;
    u32 restoreDelay;
    void *restoreTarget;
};
struct SkelGroup {
    u8 unknown0[0x40];
    u16 flags414, field416, field418;
    u8 unknown46[2];
    void *entryControl41C;
    u8 unknown4C[0x28];
    struct SkelRouteLink link;
    u8 unknown80[0xC];
    struct SkelSaved saved;
    u8 unknownA0[0x15C];
    void *routeObject;
    struct ContactPosition *destination;
    u8 direction, savedDirection, timer, unknown207;
    u8 unknown208[0x14];
    void *control;
    u8 unknown220[4];
    void *failure;
};

static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
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
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
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
static inline u16 MaskFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 IsScenePaused(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}
static inline u8 TestFlag16(u16 *p, u16 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline s32 PlayerSlot(u8 *target)
{
    if (target[8] <= 3)
        return target[8];
    return -1;
}
static inline u8 MoveTo(struct SceneActorRegistryResetView *actor, void *route,
                        struct ContactPosition *position, struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        {
            u32 mask = 0x200;
            actor->flags2D4 &= ~mask;
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
                u32 mask = 0x200;
                actor->flags2D4 &= ~mask;
            }
            return 0;
        }
    }
}
static inline u8 MoveToDestination(struct SceneActorRegistryResetView *actor,
                                   struct SkelGroup *group)
{
    void *route = group->routeObject;
    struct ContactPosition *position = &actor->body.position;
    u32 mask = 0x200;
    struct ContactPosition *destination = group->destination;
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        actor->flags2D4 &= ~mask;
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        } else {
            actor->flags2D4 &= ~mask;
            return 0;
        }
    }
}
static inline s32 PickDirection(struct SceneActorRegistryResetView *actor)
{
    register u32 current asm("r7") = actor->body.direction;
    s32 i;
    u32 direction;
    asm("" : "+r"(current));
    i = 0;
    for (;;) {
        {
            const u16 *table = gRandomTable;
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            direction = (table[gRandomIndex] & 3) << 6;
        }
        if (direction != current)
            break;
        if (++i > 3) {
            direction = current;
            break;
        }
    }
    return direction;
}
static inline void Resume(void *ctx, void *groupCtx, u8 direction)
{
    struct SceneActorRegistryResetView *actor = ctx;
    struct SkelGroup *group = groupCtx;
    u32 mask = 0x400;
    if (!(((struct SceneActorRegistryResetView *)ctx)->flags2D4 & mask) &&
        !TestFlag16(&group->flags414, 4)) {
        struct SkelSaved *saved;
        group->direction = PickDirection(actor);
        saved = &group->saved;
        saved->target = actor->target;
        saved->kind = actor->field2B5;
        saved->state = actor->state2A8;
        saved->phase = actor->phase2A9;
        saved->behavior = actor->behavior2AA;
        group->savedDirection = direction;
        {
            void *target = group->control;
            SetControl(actor, 0x1B, target);
        }
        SetState(actor, 3, 1, 0);
        group->timer = 6;
    }
    sub_0807E334(ctx);
}
static inline void SetRestore(struct SkelSaved *saved, u32 state, u32 phase, u32 behavior,
                              u32 delay, void *target)
{
    saved->restoreDelay = delay;
    saved->restoreTarget = target;
    saved->restoreState = state;
    saved->restorePhase = phase;
    saved->restoreBehavior = behavior;
}
static inline void ReturnHome(void *ctx)
{
    struct SceneActorRegistryResetView *actor = ctx;
    struct SkelGroup *group = (void *)&actor->group.common;
    struct ContactPosition *destination;
    struct SkelRouteLink *link;
    u8 **target = (u8 **)&actor->tracking.target;
    u32 cell;
    s32 slot;
    u32 mask;
    destination = group->destination;
    {
        struct SkelRouteLink *home = &group->link;
        destination->x = home->point->x;
        destination->z = home->point->z;
    }
    destination->y = CellHeight(destination, CellIndex(destination));
    if (IsScenePaused()) {
        if (!*target)
            goto fail;
        slot = PlayerSlot(*target);
        if (slot < 0)
            goto fail;
        cell = gPlayers[slot]->cell->cell;
    } else {
        cell = CellIndex((struct ContactPosition *)(*target + 12));
        slot = 0;
    }
    if (cell == actor->terrainCell || (gPlayers[slot]->flags & (mask = 0x20000))) {
        u16 busy;
        mask = 0x400;
        busy = actor->flags2D4 & mask;
        if (busy)
            return;
        {
            u32 four = 4;
            group->flags414 &= ~four;
        }
        {
            u16 blocked;
            u32 m2 = 0x80;
            blocked = actor->flags2D4 & m2;
            if (blocked) {
                void *target = group->entryControl41C;
                SetControl(actor, 0x1A, target);
                SetState(actor, 0, 0, 12);
            } else {
                void *target = actor->entryTarget228;
                SetControl(actor, 1, target);
                SetState(actor, 0, 0, 5);
                destination = group->destination;
                link = &group->link;
                destination->x = link->point->x;
                destination->z = link->point->z;
                destination->y = CellHeight(destination, CellIndex(destination));
                group->destination->y =
                    CellHeight(group->destination, CellIndex(group->destination));
                group->destination->pad = 0;
            }
        }
        Resume(ctx, (void *)&((struct SceneActorRegistryResetView *)ctx)->group.common, actor->body.direction);
        actor->field2C2 = 0x20;
        return;
    } else {
        struct ContactPosition center;
        SceneActorTerrain_GetCenter(cell, &center);
        if (MoveTo(ctx, group->routeObject, &actor->body.position, &center))
            return;
    }
fail:
    Callback_InvokeActor(actor, group->failure);
}

void sub_080BFCC4(struct SceneActorRegistryResetView *actor)
{
    struct SkelGroup *group = (void *)&actor->group.common;
    if (TakeFlag2B1(actor))
        actor->body.direction = actor->tracking.heading;
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    if ((s32)actor->counter2AC > 60) {
        u16 blocked = MaskFlags(actor, 0x80);
        if (blocked) {
            SetState(actor, 0, 0, 12);
            {
                void *target = group->entryControl41C;
                SetControl(actor, 0x1A, target);
            }
            ReturnHome(actor);
        } else {
            struct ContactPosition *destination;
            struct SkelRouteLink *link;
            SetState(actor, 3, 1, 0);
            {
                void *target = SceneActorReaction_RestoreAfterDelay;
                SetControl(actor, 0xE, target);
            }
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
            {
                struct SkelSaved *saved = &group->saved;
                void *target = actor->entryTarget228;
                SetRestore(saved, 0, 0, 7, 0x1E, target);
            }
            destination = group->destination;
            link = &group->link;
            destination->x = link->point->x;
            destination->z = link->point->z;
            destination->y = CellHeight(destination, CellIndex(destination));
            if (!MoveToDestination(actor, group)) {
                SetControl(actor, 0, SceneActorControl_OnIdle);
                SetState(actor, 3, 0, 0);
            } else
                group->destination->pad = 1;
        }
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BFCC4);
#endif
