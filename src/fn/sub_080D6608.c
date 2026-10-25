#include "global.h"

#ifdef NONMATCHING
// fuzzy: 93.1%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: PRE stack-slot order and register allocation in route reacquisition.
// MATCHING_CONCESSION: register-pinning

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
            u32 index = (gUnk_03005308 + 1) & 1023;
            u32 v;
            gUnk_03005308 = index;
            index *= 2;
            v = *(const u16 *)(index + (u32)gUnk_0203B400);
            {
                register u32 mask asm("r7") = 3;
                d = (v & mask) << 6;
            }
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
    u8 *group = g;
    {
        if (((struct SceneActorRegistryResetView *)ctx)->flags2D4 & 0x400)
            return;
    }
    if (Test16((u16 *)(((u8 *)ctx) + 0x414), 4))
        return;
    group[0x204] = PickTurn(actor->direction);
    {
        struct MummySavedControl *save = (struct MummySavedControl *)(group + 0x8C);
        save->target = actor->target;
        save->kind = actor->field2B5;
        save->state = actor->state2A8;
        save->phase = actor->phase2A9;
        save->behavior = actor->behavior2AA;
    }
    group[0x205] = saved;
    Chase_SetCtl(actor, *((void **)(group + 0x21C)), 0x1B);
    Chase_SetState(actor, 3, 1, 0);
    group[0x206] = 6;
}

static inline u8 Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)(&actor->group);
    struct ChaseTarget **slot = (struct ChaseTarget **)(((u8 *)actor) + 0xDC);
    s32 cell;
    s32 player;
    Chase_SnapToRoute(*((struct ContactPosition **)(((u8 *)actor) + 0x5D4)), ((u8 *)actor) + 0x448);
    if (Chase_Test(gUnk_030053F4, 0x4800)) {
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
            Callback_InvokeActor(actor, *((void **)(group + 0x224)));

            goto done;
        }
        cell = gUnk_02000580[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if ((cell != actor->terrainCell) && (!Chase_Bits32(&gUnk_02000580[player]->flags, 0x20000)))
        goto reroute;
    if (Chase_MotionFlags(actor, 0x400))
        goto done;
    {
        struct ChaseFlags40 *flags = (void *)(group + 0x40);
        s32 mask = ~4;
        flags->bits = mask & flags->bits;
    }
    if (Chase_MotionFlags(actor, 0x80)) {
        Chase_SetCtl(actor, *((void **)(group + 0x48)), 0x1A);
        Chase_SetBehavior(actor, 12);
    } else {
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 5);
        Chase_SnapToRoute(*((struct ContactPosition **)(group + 0x200)), group + 0x74);
        {
            u8 k1 = 0x200;
            (*((struct ContactPosition **)(group + 0x200)))->y =
                Chase_HeightAt(Chase_CellAt(*((struct ContactPosition **)(group + k1))),
                               *((struct ContactPosition **)(group + 0x200)));
        }
        (*((struct ContactPosition **)(group + 0x200)))->pad = 0;
    }
    {
        u8 saved = actor->direction;
        Turn(saved, obj, (u8 *)(&actor->group));
    }
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
    goto done;
reroute: {
    struct ContactPosition *targetPointer = &target;
    s32 x = Mod(cell, *((s16 *)(gUnk_030052F4->cells + 4)));
    s32 z = Div(cell, *((s16 *)(gUnk_030052F4->cells + 4)));
    u8 *data;
    targetPointer->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gUnk_030052F4->cells + ((cell * 4) + 12);
    targetPointer->y = ((*data) & 15) << 8;
    targetPointer->z = (z << 8) + 0x80;
    if (Chase_TryRoute(actor, *((void **)(group + 0x1FC)), &actor->position, &target))
        return 1;
    Callback_InvokeActor(actor, *((void **)(group + 0x224)));
}

done:
    return 0;
}

// confidence=low: straight-path test falls back to player route reacquisition.
u8 SceneActorTerrain_CanTraverseCell(s32, s32, s32, u32);
u8 Mummy_RequestStraightRoute(struct SceneActorRegistryResetView *);
extern u16 gUnk_02000508;
static inline s32 GridWidth(void)
{
    return *(s16 *)(gUnk_030052F4->cells + 4);
}
static inline s32 GridHeight(void)
{
    return *(s16 *)(gUnk_030052F4->cells + 6);
}

static inline u8 CanReach(s32 cell, s32 goal, s32 y, u32 kind, s32 total)
{
    s32 i, col;
    if (cell < goal) {
        i = cell + 1;
        for (col = Mod(i, GridWidth()); col < GridWidth() && SceneActorTerrain_CanTraverseCell(cell, i, y, kind);
             i++, col++) {
            if (i == goal)
                return 1;
        }
        for (i = cell + GridWidth(); total > i && SceneActorTerrain_CanTraverseCell(cell, i, y, kind);
             i += GridWidth()) {
            if (i == goal)
                return 1;
        }
    } else {
        i = cell - 1;
        for (col = Mod(i, GridWidth()); col >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i--, col--) {
            if (i == goal)
                return 1;
        }
        for (i = cell - GridWidth(); i >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i -= GridWidth()) {
            if (i == goal)
                return 1;
        }
    }
    return 0;
}

static inline u8 FindPath(struct SceneActorRegistryResetView *actor, s32 goal)
{
    s32 cell = actor->terrainCell;
    s32 y = actor->position.y;
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = GridWidth() * GridHeight();
    return CanReach(cell, goal, y, kind, total);
}

u8 Mummy_TryDirectPursuit(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u8 *group = (u8 *)(&actor->group);
    s32 goal = *((u16 *)((*((u8 **)(actor->fieldDC + 0x1c))) + 6));
    if ((*((u32 *)(group + 0x64))) != gUnk_02000508)
        return 0;
    if (goal == actor->terrainCell)
        return 1;
    if (FindPath(actor, goal))
        return 1;
    if ((!(*((u16 *)(body + 0x6d2)))) && (actor->terrainCell == goal)) {
        Mummy_RequestStraightRoute(actor);
        return 0;
    }
    actor->fieldDC = ((u32)gUnk_02000580[0]) + 0x24;
    Arrive(actor);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Mummy_TryDirectPursuit);
#endif
