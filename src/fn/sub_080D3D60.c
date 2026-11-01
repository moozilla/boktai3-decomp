// MATCHING_CONCESSION: register-pinning
#include "global.h"

// confidence=low: control reacquires a player route once the context delay clears.

#include "scene_actor_chase.h"
static inline void SetHalf(u16 *field, u32 bits)
{
    s16 mask = bits;
    mask |= *field;
    *field = mask;
}
static inline u32 ClearWordValue(u32 *field, u32 bits)
{
    return *field & ~bits;
}
#include "scene_actor_animation.h"
static inline u16 MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 *field = &actor->flags2D4;
    u32 value = *field;
    return value & mask;
}

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
            const u16 *table = gRandomTable;
            u32 index;
            u32 v;
            /* Temporary concession: retain the table address as a loop value. */
            asm("" : "+r"(table));
            index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            index *= 2;
            v = *((const u16 *)(index + (u32)table));
            d = (v & 3) << 6;
        }
        if (d != dir)
            return d;
    }
    return dir;
}

struct MummyTurnBytes {
    u8 direction : 8;
    u8 saved : 8;
    u8 remaining : 8;
};
struct MummySavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};
static inline void Turn(u8 saved, void *ctx)
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
    ((struct MummyTurnBytes *)(group + 0x204))->direction = PickTurn(actor->body.direction);
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
static inline s32 PlayerSlot(u8 *target)
{
    if (target[8] <= 3)
        return target[8];
    return -1;
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
        struct ChaseTarget *t = *slot;
        if (!t)
            goto invoke;
        player = PlayerSlot((u8 *)t);
        if (player < 0) {
            goto invoke;
        }
        cell = gPlayers[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell && !Chase_Bits32(&gPlayers[player]->flags, 0x20000))
        goto reroute;
    if (MotionFlags(actor, 0x400))
        return 0;
    {
        struct ChaseFlags40 *flags = (void *)(group + 0x40);
        s32 mask = ~4;
        flags->bits = mask & flags->bits;
    }
    if (MotionFlags(actor, 0x80)) {
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
        Turn(saved, obj);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->field2C2 = 0x20;
    return 0;
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
        goto moved;
    goto invoke;
}
invoke:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
    return 0;
moved:
    return 1;
}

// confidence=low: control callback reacquires the player and resumes a route.
u8 BouncingObject_RefreshNearbyHeading(struct SceneActorRegistryResetView *);
static inline u8 TakeControlEntry(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}
void Mummy_UpdatePlayerRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u8 *group = (u8 *)(&actor->group.common);
    if (TakeControlEntry(actor)) {
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        mask = 4;
        actor->flags2D0 |= mask;
    }
    if (BouncingObject_RefreshNearbyHeading(actor))
        Chase_SetBehavior(actor, 16);
    if (actor->behavior2AA == 16)
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    else if (actor->behavior2AA == 18)
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    actor->field2C2 = 32;
    if (!(*((u16 *)(body + 0x6d6)))) {
        actor->tracking.target = ((u32)gPlayers[0]) + 0x24;
        {
            u32 mask = 4;
            SetHalf((u16 *)(group + 0x40), mask);
        }
        actor->flags2D0 = ClearWordValue(&actor->flags2D0, 4);
        {
            struct ChaseFlags40 *field = (void *)(body + 0x6f2);
            s32 mask = ~0x1400;
            field->bits = mask & field->bits;
        }
        if (Arrive(actor)) {
            Chase_SetCtl(actor, *((void **)(group + 0x48)), 0x25);
            Chase_SetBehavior(actor, 12);
        }
    } else
        actor->counter2AC++;
}
