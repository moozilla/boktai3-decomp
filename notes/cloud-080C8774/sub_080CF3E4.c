#include "global.h"
/* Golem_ArriveAndRestore: on entry hands control to the directional-entry reset (kind 0x2C); once that finishes, faces forward, restores after a delay (following up with the chase when flags2D4 & 0x80, else the entry target) and re-targets the chased player, rerouting toward its cell or turning to a new random direction (provisional name) */
#define Golem_ArriveAndRestore sub_080CF3E4
#include "scene_actor_chase.h"

void Skeleton_ClearDirectionalEntryFlag(void);

static inline void SaveDelay(u8 *saved, void *target, u32 behavior, u32 delay)
{
    *(u32 *)(saved + 0xC) = delay;
    *(void **)(saved + 0x10) = target;
    saved[8] = 0;
    saved[9] = 0;
    saved[10] = behavior;
}

static inline void Arrive_BeginTurn(void *obj, u8 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    if (!Chase_MotionFlags(actor, 0x400) && !Chase_Test(*(u16 *)((u8 *)actor + 0x414), 4)) {
        s32 prior = actor->direction;
        s32 tries = 0;
        s32 choice;
        do {
            choice = Chase_RandomBits(3) << 6;
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

static inline u8 Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    Chase_SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    if (Chase_Test(gUnk_030053F4, 0x4800)) {
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
        cell = gUnk_02000580[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell && !Chase_Bits32(&gUnk_02000580[player]->flags, 0x20000))
        goto reroute;
    if (Chase_MotionFlags(actor, 0x400))
        goto done;
    ((struct ChaseFlags40 *)(group + 0x40))->bits &= ~4;
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
        u8 saved = actor->direction;
        Arrive_BeginTurn(obj, saved);
    }
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
    goto done;
reroute: {
    struct ContactPosition *targetPointer = &target;
    s32 x = Mod(cell, *(s16 *)(gUnk_030052F4->cells + 4));
    s32 z = Div(cell, *(s16 *)(gUnk_030052F4->cells + 4));
    u8 *data;
    targetPointer->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gUnk_030052F4->cells + (cell * 4 + 12);
    targetPointer->y = (*data & 15) << 8;
    targetPointer->z = (z << 8) + 0x80;
    if (Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position, targetPointer))
        return 1;
}
invoke:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
done:
    return 0;
}

void sub_080CF3E4(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    if (Chase_TakeArrived(actor)) {
        Chase_SetCtl(actor, Skeleton_ClearDirectionalEntryFlag, 0x2C);
        actor->field2B0 = 0;
    }
    if (!actor->field2B0)
        return;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
    if (Chase_MotionFlags(actor, 0x80))
        SaveDelay(group + 0x8C, *(void **)(group + 0x48), 0xC, 0x78);
    else
        SaveDelay(group + 0x8C, actor->entryTarget228, 7, 0x78);
    {
        u32 mask = 0x10;
        actor->flags2CC |= mask;
    }
    actor->field2C2 = 0x20;
    Chase_SetState(actor, 3, 1, 0);
    Arrive(actor);
}
