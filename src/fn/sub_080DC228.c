// MATCHING_CONCESSION: register-pinning
#include "global.h"

// confidence=low: resumes pursuit from the route point, with a delayed control transition.

#ifdef NONMATCHING
// fuzzy: 90.0%
// Still differs in turn-loop allocation, PRE stack slots, terrain temporaries, and flag-load order.
#include "scene_actor_chase.h"

void SceneActor_UpdateDirectionalAnimation(void);

struct MummyPursuitContext {
    u8 unknown0[0x6F2];
    s16 flags6F2 : 16;
};
struct MummyDelayedControl {
    u8 unknown0[8];
    u8 state, phase, behavior, unknownB;
    u32 delay;
    void *target;
};

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
        s32 prior = actor->body.direction;
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

static inline u8 Mummy_Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
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
    if (Chase_MotionFlags(actor, 0x400))
        return 0;
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
        u8 saved = actor->body.direction;
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
    if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position, targetPointer)) {
    invoke:
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
        return 0;
    }
    return 1;
}
    return 0;
}

void Mummy_ResumePlayerPursuit(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    struct MummyPursuitContext *body = actor->context;
    if (Chase_TakeArrived(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Chase_SetCtl(actor, SceneActor_UpdateDirectionalAnimation, 3);
    }
    if (actor->field2B0) {
        body->flags6F2 &= ~0x800;
        actor->tracking.target = (u32)gPlayers[0] + 0x24;
        if (Mummy_Arrive(actor)) {
            struct MummyDelayedControl *packet;
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
            packet = (void *)(group + 0x8C);
            {
                void *target = *(void **)(group + 0x48);
                u32 behavior = 12;
                packet->delay = 30;
                packet->target = target;
                packet->state = 0;
                packet->phase = 0;
                packet->behavior = behavior;
            }
            Chase_SetState(actor, 3, 1, 0);
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Mummy_ResumePlayerPursuit);
#endif
