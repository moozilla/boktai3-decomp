#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fuzzy: 76.7% (agent draft, in progress; see notes/cloud-08122DF4.md)
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
extern struct ApproachPlayer *gUnk_02000580[4];

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
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
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
static inline u8 IsScenePaused(void)
{
    if (gUnk_030053F4 & 0x4800)
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
    s32 slot = -1;
    if (target[8] <= 3)
        slot = target[8];
    return slot;
}
static inline u8 MoveTo(struct SceneActorRegistryResetView *actor, struct ApproachGroup *group,
                        struct ContactPosition *destination)
{
    void *route = group->routeObject;
    struct ContactPosition *position = &actor->position;
    s32 result;
    if (!sub_0821F204(route, position, destination)) {
        {
            s32 m = ~0x200;
            actor->flags2D4 &= m;
        }
        return 0;
    } else {
        result = sub_0821F7A8(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
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
static inline u8 MoveTo2(struct SceneActorRegistryResetView *actor, void *route,
                         struct ContactPosition *position, struct ContactPosition *destination)
{
    s32 result;
    if (!sub_0821F204(route, position, destination)) {
        {
            s32 m = ~0x200;
            actor->flags2D4 &= m;
        }
        return 0;
    } else {
        result = sub_0821F7A8(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
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
    struct ContactPosition *position = &actor->position;
    struct ContactPosition *destination = (void *)(*(u8 **)&actor->fieldDC + 12);
    s32 result;
    if (!sub_0821F204(route, position, destination)) {
        {
            s32 m = ~0x200;
            actor->flags2D4 &= m;
        }
        return 0;
    } else {
        result = sub_0821F7A8(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
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
static inline u8 PickDirection(struct SceneActorRegistryResetView *actor)
{
    u32 current = actor->direction;
    s32 i;
    for (i = 0; i <= 3; i++) {
        u32 direction;
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
        direction = (gUnk_0203B400[gUnk_03005308] & 3) << 6;
        if (direction != current)
            return direction;
    }
    return current;
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
        SetControl(actor, 0x1B, group->control);
        SetState(actor, 3, 1, 0);
        group->timer = 6;
    }
    sub_0807E334(raw);
}
static inline u8 ReturnHome(void *p)
{
    struct SceneActorRegistryResetView *actor = p;
    struct ApproachGroup *group = (void *)&actor->group;
    struct ContactPosition *destination;
    u8 **target = (u8 **)&actor->fieldDC;
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
        slot = PlayerSlot(*target);
        if (slot < 0)
            goto fail;
        cell = gUnk_02000580[slot]->cell->cell;
    } else {
        cell = CellIndex((struct ContactPosition *)(*target + 12));
        slot = 0;
    }
    if (cell == actor->terrainCell || (gUnk_02000580[slot]->flags & (mask = 0x20000))) {
        mask = 0x400;
        test = actor->flags2D4 & mask;
        if (test)
            return 0;
        {
            u16 *f = &group->flags414;
            s32 m = ~4;
            *f &= m;
        }
        mask = 0x80;
        test = actor->flags2D4 & mask;
        if (test) {
            {
                void *control = group->entryControl41C;
                SetControl(actor, 0x1A, control);
            }
            SetState(actor, 0, 0, 12);
        } else {
            {
                void *control = actor->entryTarget228;
                SetControl(actor, 1, control);
            }
            SetState(actor, 0, 0, 5);
            destination = group->destination;
            destination->x = group->link.point->x;
            destination->z = group->link.point->z;
            destination->y = CellHeight(destination, CellIndex(destination));
            group->destination->y = CellHeight(group->destination, CellIndex(group->destination));
            group->destination->pad = 0;
        }
        Resume(p, actor->direction);
        actor->field2C2 = 0x20;
        return 0;
    } else {
        struct ContactPosition center;
        SceneActorTerrain_GetCenter(cell, &center);
        if (!MoveTo2(p, group->routeObject, &actor->position, &center)) {
        fail:
            Callback_InvokeActor(actor, group->failure);
            return 0;
        }
        return 1;
    }
}
static inline s32 Quarter(s32 d)
{
    if (d >= 0)
        return d >> 2;
    return -(-d >> 2);
}
static inline s32 Quarter2(s32 value, s16 *anchor)
{
    s32 d = value - *anchor;
    if (d >= 0)
        return d >> 2;
    return -(-d >> 2);
}
static inline s32 Quarter3(s16 *value, s16 *anchor)
{
    s32 d = *value - *anchor;
    if (d >= 0)
        return d >> 2;
    return -(-d >> 2);
}
static inline s32 Clamp(s32 d)
{
    if (d > 200)
        return 200;
    if (d < -200)
        return -200;
    return d;
}
static inline u8 IsSettled(s32 dx, s32 dz)
{
    if (dx == 0 && dz == 0)
        return 1;
    return 0;
}

static inline void Approach(struct SceneActorRegistryResetView *actor)
{
    struct ApproachGroup *group = (void *)&actor->group;
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
    actor->motion.x += dx;
    actor->motion.z += dz;
    *(s16 *)((u8 *)actor + 0x318) += dx;
    *(s16 *)((u8 *)actor + 0x31C) += dz;
    if (IsSettled(dx, dz)) {
        u8 *context = actor->context;
        u32 *flags2CC;
        u32 mask;
        actor->field2B0 = 1;
        {
            s32 m = ~0x120;
            actor->flags2D4 &= m;
        }
        flags2CC = &actor->flags2CC;
        {
            s32 m = ~0x80;
            *flags2CC &= m;
        }
        if (actor->field2DC & 0x80000) {
            u32 mask = 2;
            u16 *flags = &actor->flags2D6;
            *flags = mask | *flags;
            return;
        }
        mask = 0x80;
        if (actor->flags2D4 & mask) {
            *flags2CC |= mask;
            group->flags414 |= 4;
            actor->fieldDC = (u32)gUnk_02000580[0] + 0x24;
            if (!ReturnHome(actor)) {
                if (!Chase(actor, group)) {
                    Callback_InvokeActor(actor, group->failure);
                    return;
                }
            }
            {
                void *control = group->entryControl41C;
                SetControl(actor, 0x1A, control);
            }
            SetState(actor, 0, 0, 12);
        } else {
            actor->fieldDC = (u32)gUnk_02000580[0] + 0x24;
            {
                u32 mask = 0x10;
                actor->flags2CC |= mask;
            }
            group->destination->pad = 0;
            if (!ReturnHome(actor)) {
                *(u16 *)(context + 0x6F0) = 0;
                if (!Chase(actor, group)) {
                    Callback_InvokeActor(actor, group->failure);
                    return;
                }
            }
            {
                void *control = actor->entryTarget228;
                SetControl(actor, 1, control);
            }
            SetState(actor, 0, 0, 7);
            actor->field2C2 = 0x20;
        }
    }
}

void sub_080BBDD0(struct SceneActorRegistryResetView *actor)
{
    Approach(actor);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BBDD0);
#endif
