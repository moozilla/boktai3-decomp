// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 92.3%
// fix.py best version (reprinted C)
// confidence=low: control waits sixty frames before restoring or choosing a route.

// Remaining differences: entry flag-load order, route selection registers and random-turn hoisting.
#include "scene_actor_chase.h"

static inline u8 Test16(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline u32 PickTurn(u32 dir)
{
    u32 d;
    s32 i;
    for (i = 0; i < 4; i++) {
        {
            u32 index = (gRandomIndex + 1) & 1023;
            u32 v;
            gRandomIndex = index;
            index *= 2;
            v = *(const u16 *)(index + (u32)gRandomTable);
            d = (v & 3) << 6;
        }
        if (d != dir)
            return d;
    }
    return dir;
}
struct MummySavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};
static inline void Turn(u8 saved, void *ctx, void *g)
{
    struct SceneActorRegistryResetView *actor = ctx;
    u8 *group = (u8 *)&actor->group.common;
    {
        u32 mask = 0x400;
        if (((struct SceneActorRegistryResetView *)ctx)->flags2D4 & mask)
            return;
    }
    if (Test16((u16 *)((u8 *)ctx + 0x414), 4))
        return;
    group[0x204] = PickTurn(actor->body.direction);
    {
        struct MummySavedControl *save = (struct MummySavedControl *)(group + 0x8C);
        save->target = actor->target;
        save->kind = actor->field2B5;
        save->state = actor->state2A8;
        save->phase = actor->phase2A9;
        save->behavior = actor->behavior2AA;
    }
    group[0x205] = saved;
    Chase_SetCtl(actor, *(void **)(group + 0x21C), 0x1B);
    Chase_SetState(actor, 3, 1, 0);
    group[0x206] = 6;
}
static inline u8 Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    Chase_SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    if (Chase_Test(gEngineFlags, 0x4800)) {
        register struct ChaseTarget *t asm("r1") = *slot;
        if (!t)
            goto invoke;
        {
            u32 index = t->index;
            player = -1;
            if (index <= 3)
                player = index;
        }
        if (player < 0) {
        invoke:
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            goto done;
        }
        cell = gPlayers[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell && !Chase_Bits32(&gPlayers[player]->flags, 0x20000))
        goto reroute;
    if (Chase_MotionFlags(actor, 0x400))
        goto done;
    {
        struct ChaseFlags40 *flags = (void *)(group + 0x40);
        s32 mask = ~4;
        flags->bits = mask & flags->bits;
    }
    if (Chase_MotionFlags(actor, 0x80)) {
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
    } else {
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 5);
        Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
        (*(struct ContactPosition **)(group + 0x200))->y =
            Chase_HeightAt(Chase_CellAt(*(struct ContactPosition **)(group + 0x200)),
                           *(struct ContactPosition **)(group + 0x200));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    }
    {
        u8 saved = actor->body.direction;
        Turn(saved, obj, (u8 *)&actor->group.common);
    }
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
    goto done;
reroute: {
    struct ContactPosition *targetPointer = &target;
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    targetPointer->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    targetPointer->y = (*data & 15) << 8;
    targetPointer->z = (z << 8) + 0x80;
    if (Chase_TryRoute(obj, *(void **)(group + 0x1FC), &actor->body.position, &target))
        return 1;
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
done:
    return 0;
}

// confidence=low: delayed return to route with a target-cell special action.
u8 Mummy_RequestStraightRoute(struct SceneActorRegistryResetView *);
void SceneActorMovement_ResolveRoutePosition(struct SceneActorRegistryResetView *);
static inline void SaveDelay(u8 *saved, void *target, u32 behavior, u32 delay)
{
    *(u32 *)(saved + 0xC) = delay;
    *(void **)(saved + 0x10) = target;
    saved[8] = 0;
    saved[9] = 0;
    saved[10] = behavior;
}
struct MummyRoute {
    u8 unknown0[2];
    u8 choice, step;
    u8 *definition;
    void *point;
};
static inline void PickRoute(struct SceneActorRegistryResetView *actor)
{
    struct MummyRoute *route = actor->body.currentRoute;
    u8 pick = Mod((s32)Chase_RandomBits(0xffff) >> 3, route->definition[0]);
    if (pick == route->choice)
        pick = Mod(pick + 1, route->definition[0]);
    route->choice = pick;
    route->step = 0;
    {
        u8 *base = gTerrainMap->routes;
        route->point = base + *(u16 *)(route->definition + 2) + route->choice * 8;
    }
}
void Mummy_ResumeDelayedRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group.common);
    u8 *body = actor->context;
    if (Chase_TakeArrived(actor)) {
        Chase_SetCtl(actor, actor->savedTarget, 0);
        Chase_ClearMotionFlags(actor, 0x200);
        actor->flags2CC &= ~0x80;
        *((u16 *)(group + 0x54)) = 0;
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if (((s32)actor->counter2AC) > 60) {
        {
            struct ChaseFlags40 *flags = (void *)(group + 0x40);
            s32 mask = ~6;
            flags->bits = mask & flags->bits;
        }
        Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 14);
        SaveDelay(group + 0x8c, *((void **)(group + 0x48)), 12, 30);
        Chase_SetState(actor, 3, 1, 0);
        if ((*((u16 *)(body + 0x6d2))) &&
            (actor->terrainCell == (*((u16 *)((*((u8 **)((u8 *)actor->tracking.target + 0x1c))) + 6))))) {
            if (Mummy_RequestStraightRoute(actor))
                return;
            PickRoute(actor);
            SceneActorMovement_ResolveRoutePosition(actor);
            {
                u16 mask = 4;
                *((u16 *)(group + 0x40)) |= mask;
            }
        } else
            Arrive(actor);
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", Mummy_ResumeDelayedRoute);
#endif
