// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 84.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: navigation probe events select turns or acceleration behaviors.

// Remaining differences: motion-flags pointer reuse, PRE spill order and random-turn allocation.
#include "scene_actor_chase.h"
static inline u32 PlayerFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u32 ActorMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 Test16(u16 *field, u16 mask)
{
    if ((*field) & mask)
        return 1;
    return 0;
}

// confidence=low: consumes navigation probe events and schedules descriptor turns.
s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, s32, s32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, s32, s32, s32);
u8 sub_0807AF94(void *);
u8 Mummy_RequestStraightRoute(void *);
static inline u32 Mummy_RandomBits(s16 mask)
{
    return gRandomTable[gRandomIndex = (gRandomIndex + 1) & 1023] & mask;
}

static inline void BeginProbeTurn(void *obj, u32 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    if (!ActorMotionFlags(actor, 0x400) && !Test16((u16 *)((u8 *)actor + 0x414), 4)) {
        s32 prior = actor->body.direction;
        s32 tries = 0;
        s32 choice;
        do {
            choice = Mummy_RandomBits(3) << 6;
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
        group[0x206] = 2;
    }
}

void Mummy_UpdateProbeNavigation(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group.common);
    u8 *body = actor->context;
    register s32 speed asm("r6");
    if (Chase_TakeArrived(actor)) {
        u16 *field = (void *)(body + 0x6f2);
        u32 mask = 0xffffefff;
        u32 value = *field;
        mask &= value;
        *field = mask;
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    speed = Callback_InvokeActor(actor, *((void **)(((u8 *)actor) + 0x260)));
    {
        u32 mask = 0xa02f;
        u16 *scale;
        if (actor->flags2D4 & mask) {
            actor->field2C2 = 0x40;
            scale = &actor->field2C2;
        } else {
            u16 value = actor->field2C2;
            scale = &actor->field2C2;
            if (!value)
                *scale = 0x40;
        }
        if (actor->drawableKind == 0)
            Anim_SetDurationScale(((u8 *)actor->drawable) + 0x48, *scale);
        else
            Anim_SetFrameDurationScale(((u8 *)actor->drawable) + 0x20, *scale);
    }
    if (NavMesh_ConsumeProbeEvent(*((void **)(group + 0x1fc)))) {
        if (!SceneActorBehavior_RequestDescriptorTurn(actor, *((void **)(group + 0x1fc)), 32, 32,
                                                      speed))
            goto increment;
        if (!Test16((u16 *)(group + 0x40), 4)) {
            if (sub_0807AF94(actor))
                return;
            if (Test16((u16 *)(group + 0x40), 1)) {
                u32 saved = ((((u8 *)actor)[0x3cd] + 5) & 7) << 5;
                BeginProbeTurn(actor, saved);
                return;
            }
        }
        Chase_SetBehavior(actor, 13);
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, *((void **)(group + 0x1fc)), 32,
                                                             32, speed)) {
        if (PlayerFlags(&gPlayers[0]->flags, 8) && (!(*((u16 *)(body + 0x6d2))))) {
            if (Mummy_RequestStraightRoute(actor))
                return;
        }
        Chase_SetBehavior(actor, 14);
        return;
    }
increment:
    actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", Mummy_UpdateProbeNavigation);
#endif
