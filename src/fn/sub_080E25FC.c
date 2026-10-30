// MATCHING_CONCESSION: register-pinning
// confidence=low: reaction flags select special control, chase, route recovery or a timed turn.
#include "global.h"
// confidence=low: chooses chase or route recovery after an actor reaction.

#ifdef NONMATCHING
// fuzzy: 84.6%
// Branch layout, flag-address lifetimes and register allocation still differ.
#include "scene_actor_chase.h"
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
        register s32 prior asm("r7") = actor->body.direction;
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
    if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position, targetPointer)) {
    invoke:
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
        return 0;
    }
    return 1;
}
done:
    return 0;
}
#include "scene_actor_animation.h"
void Sound_PlaySong(u32);
u8 Dog_IsActionTimerExpired(struct SceneActorRegistryResetView *);
u8 sub_080DEF78(struct SceneActorRegistryResetView *, u32);
void SceneActorControl_OnIdle(void);

static inline void StartReactionAnimation(struct SceneActorRegistryResetView *a)
{
    if (!a->group.hitReactionLatch && (a->field2E6 & 8)) {
        a->field2B8 = 45;
        a->group.hitReactionLatch = 65;
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 3);
    }
}

s32 Dog_ChooseReactionBehavior(struct SceneActorRegistryResetView *a)
{
    u8 *context = a->context;
    u8 *group = (u8 *)&a->group.common;
    if (a->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    if (Dog_ReadMotionFlags(a, 0x8002)) {
        {
            s32 mask = -33;
            a->flags2D4 = mask & a->flags2D4;
        }
        return 0;
    }
    if (a->field2DC & 0xC0000) {
        Chase_SetCtl(a, a->reactionTarget230, 18);
        Chase_SetState(a, 3, 0, 0);
        {
            u32 *flags = &a->flags2D0;
            u32 mask = ~0x1000000;
            *flags &= mask;
        }
        {
            u32 *flags = (u32 *)(context + 0x678);
            s32 mask = -9;
            *flags &= mask;
        }
        return 1;
    }
    {
        s32 mask = -33;
        a->flags2D4 = mask & a->flags2D4;
    }
    if ((a->flags2D4 & 0x100) || a->field2B8)
        return 0;
    if (a->flags2D4 & 0x80) {
        StartReactionAnimation(a);
        if (!a->tracking.target)
            return 0;
        {
            u32 mask = 0x80;
            a->flags2CC |= mask;
        }
        *(u16 *)(group + 0x40) = 4 | *(u16 *)(group + 0x40);
        a->tracking.target = (u32)gPlayers[0] + 0x24;
        if (Dog_IsActionTimerExpired(a)) {
            if (!Dog_RerouteToTarget(a)) {
                if (!Chase_TryRoute(a, *(void **)(group + 0x1FC), &a->body.position,
                                    &a->tracking.target->position)) {
                    Callback_InvokeActor(a, *(void **)(group + 0x224));
                    return 0;
                }
            }
            Chase_SetCtl(a, *(void **)(group + 0x48), 26);
            Chase_SetBehavior(a, 12);
            return 1;
        }
        if (context[0x6A8])
            return 1;
        Chase_SetCtl(a, SceneActorControl_OnIdle, 16);
        Chase_SetBehavior(a, 18);
        {
            s32 angle = (a->tracking.heading + 0xA0) & 255;
            if (angle >= 0)
                angle >>= 5;
            else
                angle = -(-angle >> 5);
            ((struct SceneActorAnimationView *)a)->nextDirection = (angle + 3) & 7;
        }
        {
            u32 direction = a->body.direction;
            a->body.direction = (((direction + 0xA0) & 255) >> 6) << 6;
            context[0x6A8] = 90;
            return (u8)sub_080DEF78(a, direction) ? 1 : 0;
        }
    } else {
        StartReactionAnimation(a);
        if (!a->tracking.target)
            return 0;
        a->tracking.target = (u32)gPlayers[0] + 0x24;
        {
            u32 mask = 16;
            a->flags2CC |= mask;
        }
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        if (!Dog_RerouteToTarget(a)) {
            if (!Chase_TryRoute(a, *(void **)(group + 0x1FC), &a->body.position,
                                &a->tracking.target->position)) {
                Callback_InvokeActor(a, *(void **)(group + 0x224));
                return 0;
            }
        }
        Chase_SetCtl(a, a->entryTarget228, 1);
        Chase_SetBehavior(a, 7);
        a->field2C2 = 16;
        return 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Dog_ChooseReactionBehavior);
#endif
