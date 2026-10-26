#include "global.h"

#include "scene_actor_chase.h"

// confidence=low: chase-state step; arrives at the route point, starts the next turn or reroutes, and sets up the state machine.
u8 sub_0812F3CC(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, s32, s32, u8);

#ifdef NONMATCHING
// fuzzy: 38.6%
// scope shape 775 (differences once low registers are set aside)
// confidence=low: chase-state step; arrives at the route point, starts the next turn or reroutes, and sets up the state machine.
void sub_080AF2BC(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    struct ContactPosition *p;
    s32 cell;
    s32 player;

    Chase_TakeArrived(actor);
    if (Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x270)))
        return;
    if (sub_0812F3CC(actor))
        return;
    if (actor->counter2AC <= 0x3c)
        actor->counter2AC++;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, *((u8 *)actor + 0x98));
    Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 0xE);
    Chase_SetState(actor, 3, 1, 0);

    p = *(struct ContactPosition **)((u8 *)actor + 0x5D4);
    if (Chase_MotionFlags(actor, 0x80)) {
        *(u32 *)(group + 0x8C + 0xC) = 0x1E;
        *(void **)(group + 0x8C + 0x10) = *(void **)(group + 0x48);
        group[0x8C + 8] = 0;
        group[0x8C + 9] = 0;
        group[0x8C + 0xA] = 12;
        Chase_SnapToRoute(p, (u8 *)actor + 0x448);
    } else {
        *(u32 *)(group + 0x8C + 0xC) = 0x1E;
        *(void **)(group + 0x8C + 0x10) = p;
        group[0x8C + 8] = 0;
        group[0x8C + 9] = 0;
        group[0x8C + 0xA] = 7;
        Chase_SnapToRoute(p, (u8 *)actor + 0x448);
        Chase_ClearMotionFlags(actor, 0x80);
    }

    if (Chase_Test(gEngineFlags, 0x4800)) {
        struct ChaseTarget *t = *(struct ChaseTarget **)((u8 *)actor + 0xDC);
        if (!t)
            goto invoke;
        player = t->index <= 3 ? t->index : -1;
        if (player < 0) {
        invoke:
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return;
        }
        cell = gUnk_02000580[player]->cell[3];
    } else {
        struct ChaseTarget *t = *(struct ChaseTarget **)((u8 *)actor + 0xDC);
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }

    if (cell != actor->terrainCell && !Chase_Bits32(&gUnk_02000580[player]->flags, 0x20000))
        goto reroute;
    if (Chase_MotionFlags(actor, 0x400))
        return;
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
    Chase_BeginTurn(actor, actor->direction);
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
    return;

reroute: {
    struct ContactPosition target;
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    target.x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    target.y = (*data & 15) << 8;
    target.z = (z << 8) + 0x80;
    if (Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position, &target))
        return;
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080AF2BC);
#endif
