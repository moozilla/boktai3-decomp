#include "global.h"
#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fuzzy: 87.9%
// fix.py best version (reprinted C)
// confidence=low: waits twenty updates, saves the group behavior and attempts route recovery.
// confidence=low: waits twenty updates before saving chase behavior and rerouting.

// Recovery inline register allocation and several flag updates still differ.
static inline void Dog_ClearGroupBits(u8 *group, u32 mask)
{
    ((struct ChaseFlags40 *)(group + 0x40))->bits &= ~mask;
}
static inline u16 Dog_ReadMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 value = actor->flags2D4;
    return value & mask;
}
static inline void Dog_BeginRecoveryTurn(void *obj, u8 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    if (!Dog_ReadMotionFlags(actor, 0x400) && !Chase_Test(*(u16 *)((u8 *)actor + 0x414), 4)) {
        s32 prior = actor->body.direction;
        s32 tries = 0;
        s32 choice;
        const u16 *table = gRandomTable;
        do {
            u32 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            choice = (table[index] & 3) << 6;
            if (choice != prior)
                goto chosen;
        } while (++tries <= 3);
        choice = prior;
    chosen:
        group[0x204] = choice;
        Chase_SavePacket(group + 0x8C, actor);
        group[0x205] = saved;
        Chase_SetCtl(actor, *(void **)(group + 0x21C), 0x1B);
        Chase_SetState(actor, 3, 1, 0);
        group[0x206] = 6;
    }
}
static inline u8 Dog_RerouteToTarget(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    Chase_SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    if (Chase_Test(gEngineFlags, 0x4800)) {
        struct ChaseTarget *t = *slot;
        if (!t)
            goto invoke;
        {
            u32 index = t->index;
            player = -1;
            if (index <= 3)
                player = index;
        }
        if (player < 0)
            goto invoke;
        cell = gPlayers[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell && !Chase_Bits32(&gPlayers[player]->flags, 0x20000))
        goto reroute;
    if (Dog_ReadMotionFlags(actor, 0x400))
        goto done;
    Dog_ClearGroupBits(group, 4);
    if (Dog_ReadMotionFlags(actor, 0x80)) {
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
        Dog_BeginRecoveryTurn(obj, saved);
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
    if (Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position, targetPointer))
        return 1;
invoke:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
done:
    return 0;
}
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
static inline void Chase_SetState_(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    u32 behavior = b;
    actor->phase2A9 = p;
    actor->state2A8 = s;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u32 Arg1(u32 k_, u8 *group)
{
    return group + k_;
}

void Dog_BeginDelayedRouteRecovery(struct SceneActorRegistryResetView *a)
{
    u8 *group = (u8 *)(&a->group.common);
    if (Chase_TakeArrived(a)) {
        Chase_SetCtl(a, a->savedTarget, 0);
        {
            u32 *flags = &a->flags2CC;
            s32 mask = -129;
            *flags &= mask;
        }
        Chase_ClearMotionFlags(a, 0x200);
    }
    if ((u8)Callback_InvokeActor(a, a->contactQuery.opaque))
        return;
    if (sub_0812F3CC(a))
        return;
    if (((s32)a->counter2AC) > 20) {
        {
            u16 *flags = (u16 *)Arg1(0x40, group);
            s32 mask = -7;
            *flags = mask & (*flags);
        }
        Chase_SetCtl(a, SceneActorReaction_RestoreAfterDelay, 14);
        {
            u32 duration = *((u16 *)(group + 0x46));
            void *control = *((void **)(group + 0x48));
            u8 *packet = group + 0x8C;
            *((u32 *)(packet + 12)) = duration;
            *((void **)(packet + 16)) = control;
            packet[8] = 0;
            packet[9] = 0;
            packet[10] = 12;
        }
        Chase_SetState_(a, 3, 1, 0);
        Dog_RerouteToTarget(a);
    } else
        a->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", Dog_BeginDelayedRouteRecovery);
#endif
