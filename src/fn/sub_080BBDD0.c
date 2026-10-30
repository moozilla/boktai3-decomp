// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fuzzy: 90.7%
// fix.py best version (reprinted C)
// confidence=low: installed by Skeleton_Init; converges saved displacement before route/chase recovery
// Remaining differences: flag-address reuse, RNG retry allocation, and inlined recovery stack slots.
/* Drift toward the anchor at 0x318/0x31C (quarter steps, clamped to 200); once settled,
 * return to the route point or chase the player target (tentative). */
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
    if (target[8] <= 3)
        return target[8];
    return -1;
}
static inline u8 MoveTo2(struct SceneActorRegistryResetView *actor, void *route,
                         struct ContactPosition *position, struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        {
            s32 m = ~0x200;
            actor->flags2D4 &= m;
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
                s32 m = ~0x200;
                actor->flags2D4 &= m;
            }
            return 0;
        }
    }
}
static inline u8 Chase(struct SceneActorRegistryResetView *actor, struct ApproachGroup *group)
{
    void *route = group->routeObject;
    struct ContactPosition *position = &actor->body.position;
    struct ContactPosition *destination = (void *)(*(u8 **)&actor->tracking.target + 12);
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        {
            s32 m = ~0x200;
            actor->flags2D4 &= m;
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
                s32 m = ~0x200;
                actor->flags2D4 &= m;
            }
            return 0;
        }
    }
}
static inline u32 RandomBits(u32 mask)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index] & mask;
}
static inline u8 PickDirection(struct SceneActorRegistryResetView *actor)
{
    u32 current = actor->body.direction;
    s32 i;
    u8 direction;
    i = 0;
    do {
        direction = RandomBits(3) << 6;
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
    struct ApproachGroup *group = (void *)&raw->group.common;
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
    u16 value = actor->flags2D4 & mask;
    return value;
}
static inline u8 ReturnHome(void *p)
{
    struct SceneActorRegistryResetView *actor = p;
    struct ApproachGroup *group = (void *)&actor->group.common;
    struct ContactPosition *destination;
    u8 **target = (u8 **)&actor->tracking.target;
    u32 cell;
    s32 slot;
    u32 mask;
    u16 test;
    destination = actor->group.destination;
    destination->x = ((struct ApproachRouteLink *)&actor->group.homeRoute)->point->x;
    destination->z = ((struct ApproachRouteLink *)&actor->group.homeRoute)->point->z;
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
static inline s32 Quarter(s32 d)
{
    s32 result;
    if (d >= 0)
        result = d >> 2;
    else
        result = -(-d >> 2);
    return result;
}
static inline u8 IsSettled(s32 dx, s32 dz)
{
    if (dx == 0 && dz == 0)
        return 1;
    return 0;
}

static inline void ClearSettleFlags(struct SceneActorRegistryResetView *actor)
{
    struct SettleBits {
        s16 bits : 16;
    };
    ((struct SettleBits *)&actor->flags2D4)->bits &= ~0x120;
}
static inline u8 Drift(struct SceneActorRegistryResetView *actor)
{
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    dx = Quarter(*(s16 *)((u8 *)actor + 0x310) - *(s16 *)((u8 *)actor + 0x318));
    dz = Quarter(*(s16 *)((u8 *)actor + 0x314) - *(s16 *)((u8 *)actor + 0x31C));
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->body.motion.x += dx;
    actor->body.motion.z += dz;
    *(s16 *)((u8 *)actor + 0x318) += dx;
    *(s16 *)((u8 *)actor + 0x31C) += dz;
    return IsSettled(dx, dz);
}
static inline void Settle(struct SceneActorRegistryResetView *actor, struct ApproachGroup *group,
                          u8 *context)
{
    u32 *flags2CC;
    u16 test;
    u32 mask;
    typeof(mask) t1;
    ClearSettleFlags(actor);
    flags2CC = &actor->flags2CC;
    {
        s32 m = ~0x80;
        *flags2CC &= m;
    }
    if (actor->field2DC & 0x80000) {
        struct ShortFlags {
            s16 bits : 16;
        };
        {
            u32 mask = 2;
            asm("" : "+r"(mask));
            ((struct ShortFlags *)(&actor->flags2D6))->bits |= mask;
        }
        return;
    }
    mask = (t1 = 0x80);
    test = actor->flags2D4 & t1;
    if (test) {
        *flags2CC |= t1;
        group->flags414 |= 4;
        actor->tracking.target = ((u32)gPlayers[0]) + 0x24;
        if (!ReturnHome(actor)) {
            if (!Chase(actor, group)) {
                Callback_InvokeActor(actor, group->failure);
                return;
            }
        }
        {
            void *control = group->entryControl41C;
            SetControl(actor, control, 0x1A);
        }
        SetState(actor, 0, 0, 12);
    } else {
        actor->tracking.target = ((u32)gPlayers[0]) + 0x24;
        {
            u32 mask = 0x10;
            actor->flags2CC |= t1;
        }
        group->destination->pad = 0;
        if (!ReturnHome(actor)) {
            *((u16 *)(context + 0x6F0)) = 0;
            if (!Chase(actor, group)) {
                Callback_InvokeActor(actor, group->failure);
                return;
            }
        }
        {
            void *control = actor->entryTarget228;
            SetControl(actor, control, 1);
        }
        SetState(actor, 0, 0, 7);
        actor->field2C2 = 0x20;
    }
}

void Skeleton_ConvergeDisplacedPosition(struct SceneActorRegistryResetView *actor)
{
    struct ApproachGroup *group = (void *)(&actor->group.common);
    if (Drift(actor)) {
        u8 *context = actor->context;
        actor->field2B0 = 1;
        Settle(actor, group, context);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Skeleton_ConvergeDisplacedPosition);
#endif
