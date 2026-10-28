#include "global.h"

#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_contacts.h"

// confidence=low: per-frame route follower: releases its contact node, resets its reaction control, then follows its route point (flag 0x80) or pursues its route, choosing a new wander direction when blocked (provisional)

// confidence=low: per-frame route follower: releases its contact node, resets its reaction control, then follows its route point (flag 0x80) or pursues its route, choosing a new wander direction when blocked (provisional)
void sub_0807E334(void *);
void SceneActorContacts_ReleaseNode(struct SceneActorContactNode *);

struct Sub0807DB34Point {
    s16 x, z;
};
struct Sub0807DB34Link {
    u8 unknown0[8];
    struct Sub0807DB34Point *point;
};
struct Sub0807DB34Saved {
    void *target;
    u8 state, phase, behavior, kind;
};
struct Sub0807DB34Group {
    u8 unknown0[0x40];
    u16 flags414, field416, field418;
    u8 unknown46[2];
    void *entryControl41C;
    struct SceneActorRegistryControl control4C;
    u8 unknown68[0xC];
    struct Sub0807DB34Link link;
    u8 unknown80[0xC];
    struct Sub0807DB34Saved saved;
    u8 unknown94[0x168];
    void *routeObject;
    struct ContactPosition *destination;
    u8 direction, savedDirection, timer, unknown207;
    u8 unknown208[0x14];
    void *control;
    u8 unknown220[4];
    void *failure;
};

static inline void Sub0807DB34_SetControl(struct SceneActorRegistryResetView *actor, void *control,
                                          u32 kind)
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

static inline void Sub0807DB34_SetState(struct SceneActorRegistryResetView *actor, u8 state,
                                        u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void Sub0807DB34_Finish(struct SceneActorRegistryResetView *actor)
{
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
}

static inline void Sub0807DB34_Turn(struct SceneActorRegistryResetView *actor,
                                    struct Sub0807DB34Group *group)
{
    u8 oldDirection = actor->direction;
    u8 newDirection = oldDirection;
    s32 attempt;

    for (attempt = 0; attempt <= 3; attempt++) {
        u16 candidate;
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        candidate = (gRandomTable[gRandomIndex] & 3) << 6;
        if (candidate != oldDirection) {
            newDirection = candidate;
            break;
        }
    }
    group->direction = newDirection;
    group->saved.target = actor->target;
    group->saved.kind = actor->field2B5;
    group->saved.state = actor->state2A8;
    group->saved.phase = actor->phase2A9;
    group->saved.behavior = actor->behavior2AA;
    group->savedDirection = oldDirection;
    Sub0807DB34_SetControl(actor, group->control, 0x1B);
    Sub0807DB34_SetState(actor, 3, 1, 0);
    group->timer = 6;
}

static inline s32 Sub0807DB34_FollowRoute(struct SceneActorRegistryResetView *actor,
                                          struct Sub0807DB34Group *group)
{
    struct ContactPosition *destination = group->destination;
    u8 *target;
    void **table = (void **)0x02000580;
    s32 idx = 0;
    s32 cell;
    u16 cellValue;

    destination->x = group->link.point->x;
    destination->z = group->link.point->z;
    cell = SceneActorTerrain_GetCellIndex(destination);
    destination->y = SceneActorTerrain_GetHeight(cell, destination);

    if (gEngineFlags & 0x4800) {
        target = *(u8 **)&actor->fieldDC;
        if (!target)
            goto failed;
        idx = target[8];
        if (idx > 3)
            idx = -1;
        if (idx < 0)
            goto failed;
        cellValue = *(u16 *)(*(u8 **)((u8 *)table[idx] + 0x40) + 6);
    } else {
        target = *(u8 **)&actor->fieldDC;
        cell = SceneActorTerrain_GetCellIndex((struct ContactPosition *)(target + 0xC));
        cellValue = cell;
        idx = 0;
    }

    if (cellValue != actor->terrainCell && !(*(u32 *)((u8 *)table[idx] + 0x20) & 0x20000)) {
        struct ContactPosition center;
        void *route = group->routeObject;
        s32 r;

        SceneActorTerrain_GetCenter(cellValue, &center);
        if (!Nav_IsPositionInMesh(route, &actor->position, &center)) {
            actor->flags2D4 &= ~0x200;
            r = 0;
        } else {
            r = Terrain_ApplyBlocked(route, &actor->position, &center);
            if (r == 0) {
                actor->flags2D4 &= ~0x200;
            } else {
                if (r != 2)
                    Memory_Clear(&actor->motion, 8);
                r = 1;
            }
        }
        if (r)
            return 1;
        goto failed;
    }

    if (actor->flags2D4 & 0x400)
        return 0;
    group->flags414 &= ~4;
    if (actor->flags2D4 & 0x80) {
        Sub0807DB34_SetState(actor, 0, 0, 12);
        Sub0807DB34_SetControl(actor, group->entryControl41C, 0x1A);
    } else {
        struct ContactPosition *slot;
        Sub0807DB34_SetState(actor, 0, 0, 5);
        Sub0807DB34_SetControl(actor, actor->entryTarget228, 1);
        slot = group->destination;
        slot->x = group->link.point->x;
        slot->z = group->link.point->z;
        cell = SceneActorTerrain_GetCellIndex(slot);
        slot->y = SceneActorTerrain_GetHeight(cell, slot);
        cell = SceneActorTerrain_GetCellIndex(slot);
        slot->y = SceneActorTerrain_GetHeight(cell, slot);
        slot->pad = 0;
    }
    if (!((actor->flags2D4 & 0x400) || (group->flags414 & 4)))
        Sub0807DB34_Turn(actor, group);
    Sub0807DB34_Finish(actor);
    return 0;
failed:
    Callback_InvokeActor(actor, group->failure);
    return 0;
}

static inline void Sub0807DB34_Pursue(struct SceneActorRegistryResetView *actor,
                                      struct Sub0807DB34Group *group)
{
    struct ContactPosition *destination = group->destination;
    struct SceneActorReactionCommand *command =
        (struct SceneActorReactionCommand *)((u8 *)actor + 0x460);
    void *route;
    s32 result = 0;

    destination->pad = 1;
    route = *(void **)((u8 *)actor + 0x5D0);
    if (!Nav_IsPositionInMesh(route, &actor->position, destination)) {
        actor->flags2D4 &= ~0x200;
    } else {
        result = Terrain_ApplyBlocked(route, &actor->position, destination);
        if (result == 0) {
            actor->flags2D4 &= ~0x200;
        } else {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            result = 1;
        }
    }
    if (!result) {
        Callback_InvokeActor(actor, group->failure);
        return;
    }
    command->duration = 60;
    command->payload = actor->entryTarget228;
    command->first = 0;
    command->second = 0;
    command->kind = 7;
    if (actor->flags2CC & 0x200000) {
        actor->flags2D0 &= ~0x01000000;
        actor->flags2D4 &= ~0x200;
        group->control4C.timer = 0;
        group->control4C.mode = 0;
        group->control4C.counter = 0;
    }
}

#ifdef NONMATCHING
// fuzzy: 36.0%
// scope shape 743 (differences once low registers are set aside)
// confidence=low: per-frame route follower: releases its contact node, resets its reaction control, then follows its route point (flag 0x80) or pursues its route, choosing a new wander direction when blocked (provisional)
void sub_0807DB34(struct SceneActorRegistryResetView *actor)
{
    struct Sub0807DB34Group *group = (struct Sub0807DB34Group *)((u8 *)actor + 0x3D4);
    struct SceneActorReactionCommand *command =
        (struct SceneActorReactionCommand *)((u8 *)actor + 0x460);
    s32 result;

    if (actor->flags2D4 & 0x1000) {
        SceneActorContacts_ReleaseNode(*(struct SceneActorContactNode **)((u8 *)actor + 0x45C));
        actor->flags2D4 &= ~0x1000;
        *(void **)((u8 *)actor + 0x45C) = 0;
    }
    actor->field2C2 = 0x40;
    SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
    Sub0807DB34_SetControl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
    Sub0807DB34_SetState(actor, 3, 1, 0);

    if (actor->flags2D4 & 0x80) {
        result = Sub0807DB34_FollowRoute(actor, group);
        if (result) {
            command->duration = 60;
            command->payload = group->entryControl41C;
            command->first = 0;
            command->second = 0;
            command->kind = 12;
            group->flags414 &= ~4;
        }
    } else {
        Sub0807DB34_Pursue(actor, group);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807DB34);
#endif
