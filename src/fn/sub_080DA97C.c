// MATCHING_CONCESSION: register-pinning
#include "global.h"

// confidence=low: chooses a route, pursuit, or recovery response from the movement flags.

#ifdef NONMATCHING
// fuzzy: 86.5%
// Still differs in turn-loop allocation, PRE stack slots, flag-load order, and far-branch layout.
#include "scene_actor_chase.h"

void SceneActorMovement_ResolveRoutePosition(void *);
struct MummyRoutePoints {
    u8 count, unknown1;
    u16 offset;
};
struct MummyRouteSelection {
    u8 unknown0[2];
    u8 index, phase;
    struct MummyRoutePoints *points;
    void *current;
};
static inline void ChooseRoute(struct SceneActorRegistryResetView *actor)
{
    struct MummyRouteSelection *route = actor->currentRoute;
    u8 choice;
    {
        u32 index = (gRandomIndex + 1) & 1023;
        s32 value;
        gRandomIndex = index;
        index *= 2;
        value = *(const u16 *)((u32)gUnk_0203B400 + index);
        value >>= 3;
        choice = Mod(value, route->points->count);
    }
    if (choice == route->index)
        choice = Mod(choice + 1, route->points->count);
    route->index = choice;
    route->phase = 0;
    {
        u8 *base = *(void **)((u8 *)gTerrainMap + 0x10);
        base += route->points->offset;
        route->current = base + route->index * 8;
    }
}
static inline void PreparePursuitAnimation(struct SceneActorRegistryResetView *actor)
{
    if (!actor->field5CA && (actor->field2E6 & 8)) {
        actor->field2B8 = 45;
        actor->field5CA = 0x41;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    }
}
static inline u8 Mummy_TestGroup(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (*(u16 *)((u8 *)actor + 0x414) & mask)
        return 1;
    return 0;
}
static inline void Mummy_BeginTurn(void *obj, u8 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    if (!Chase_MotionFlags(actor, 0x400) && !Mummy_TestGroup(actor, 4)) {
        register s32 prior asm("r7") = actor->direction;
        s32 tries = 0;
        s32 choice;
        do {
            {
                register u32 bits asm("r1") = 3;
                choice = Chase_RandomBits(bits) << 6;
            }
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

static inline void Mummy_SetCtlZero(struct SceneActorRegistryResetView *actor, void *cb, u32 k,
                                    u32 zero)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void Mummy_SetBehaviorZero(struct SceneActorRegistryResetView *actor, u32 b, u32 zero)
{
    actor->state2A8 = zero;
    actor->phase2A9 = zero;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u8 Mummy_Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    u8 zero;
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
        cell = gUnk_02000580[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell && !Chase_Bits32(&gUnk_02000580[player]->flags, 0x20000))
        goto reroute;
    if (Chase_MotionFlags(actor, 0x400))
        return 0;
    {
        struct ChaseFlags40 *field = (void *)(group + 0x40);
        s32 mask = ~4;
        field->bits &= mask;
        zero = 0;
    }
    if (Chase_MotionFlags(actor, 0x80)) {
        Mummy_SetCtlZero(actor, *(void **)(group + 0x48), 0x1A, zero);
        Mummy_SetBehaviorZero(actor, 12, zero);
    } else {
        Mummy_SetCtlZero(actor, actor->entryTarget228, 1, zero);
        Mummy_SetBehaviorZero(actor, 5, zero);
        Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
        (*(struct ContactPosition **)(group + 0x200))->y =
            Chase_HeightAt(Chase_CellAt(*(struct ContactPosition **)(group + 0x200)),
                           *(struct ContactPosition **)(group + 0x200));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    }
    {
        u8 saved = actor->direction;
        Mummy_BeginTurn(obj, saved);
    }
    sub_0807E334(actor);
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
    if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position, targetPointer)) {
    invoke:
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
        return 0;
    }
    return 1;
}
    return 0;
}

u8 Mummy_SelectMovementResponse(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    if (!actor->reactionSoundVariant)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0xFD);
    {
        u32 *word = &actor->flags2D0;
        s32 mask = ~4;
        *word &= mask;
    }
    if (Chase_MotionFlags(actor, 0x10)) {
        ChooseRoute(actor);
        SceneActorMovement_ResolveRoutePosition(actor);
        Chase_SetBehavior(actor, 8);
        Chase_SetCtl(actor, *(void **)(group + 0x214), 0x13);
        Chase_ClearMotionFlags(actor, 0x20);
        return 1;
    }
    if (Chase_MotionFlags(actor, 0x2000)) {
        actor->direction = actor->field2B6 + 0x80;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Chase_ClearMotionFlags(actor, 0x2020);
        actor->flags2CC |= 0x10;
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        actor->fieldDC = (u32)gUnk_02000580[0] + 0x24;
        if (Mummy_Arrive(actor))
            goto pursue;
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                            &((struct ChaseTarget *)actor->fieldDC)->position))
            goto failed;
        goto pursue;
    }
    if (Chase_MotionFlags(actor, 0x8002)) {
        Chase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (Chase_Bits32(&actor->field2DC, 0xC0000)) {
        Chase_SetCtl(actor, *(void **)actor->unknown220, 0x12);
        Chase_SetState(actor, 3, 0, 0);
        actor->flags2D0 &= ~0x1000000;
        return 1;
    }
    Chase_ClearMotionFlags(actor, 0x20);
    if (Chase_MotionFlags(actor, 0x100))
        return 0;
    if (actor->field2B8)
        return 0;
    if (Chase_MotionFlags(actor, 0x80)) {
        PreparePursuitAnimation(actor);
        if (!actor->fieldDC)
            return 0;
        actor->flags2CC |= 0x80;
        *(u16 *)(group + 0x40) |= 4;
        actor->fieldDC = (u32)gUnk_02000580[0] + 0x24;
        if (!Mummy_Arrive(actor)) {
            if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                                &((struct ChaseTarget *)actor->fieldDC)->position))
                goto failed;
        }
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
        return 1;
    } else {
        PreparePursuitAnimation(actor);
        if (!actor->fieldDC)
            return 0;
        actor->flags2CC |= 0x10;
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        actor->fieldDC = (u32)gUnk_02000580[0] + 0x24;
        if (Mummy_Arrive(actor))
            goto pursue;
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                            &((struct ChaseTarget *)actor->fieldDC)->position))
            goto failed;
        goto pursue;
    }
failed:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
    return 0;
pursue:
    Chase_SetCtl(actor, actor->entryTarget228, 1);
    Chase_SetBehavior(actor, 7);
    actor->field2C2 = Div(64, 3);
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", Mummy_SelectMovementResponse);
#endif
