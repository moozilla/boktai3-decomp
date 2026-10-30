#include "global.h"
#include "scene_actor_reaction.h"
void *SceneActorTarget_FindNearbyPlayer(struct SceneActorRegistryResetView *);
s32 SceneActorReaction_FindAdjacentTarget(struct SceneActorRegistryResetView *, s32);
void Ivy_ApplyHeldTargetDamage(void *);
static inline u8 TakeReset(struct SceneActorRegistryResetView *actor)
{
    u8 *flag = &actor->field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline void ResetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                              u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = 1;
    }
}
void SceneActorReaction_AwaitTargetControl(struct SceneActorRegistryResetView *actor)
{
    void *target;
    TakeReset(actor);
    target = SceneActorTarget_FindNearbyPlayer(actor);
    if (target) {
        void *callback;
        u8 kind;
        u8 *prepared;
        u32 zero;
        u8 one;
        u8 byteZero;
        u32 state;
        actor->tracking.target = (u32)target;
        callback = Ivy_ApplyHeldTargetDamage;
        kind = 45;
        prepared = &actor->field2B2;
        zero = 0;
        one = 1;
        *prepared = one;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        {
            u16 *field = &actor->field2C8;
            byteZero = 0;
            *field = zero;
        }
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
        ResetState(actor, 3, 0, 0);
    } else {
        if ((s32)actor->counter2AC > 300 || SceneActorReaction_FindAdjacentTarget(actor, 1)) {
            u32 one = 1;
            u32 zero = (u32)target;
            void *callback;
            u32 kind;
            ResetState(actor, 0, 0, 1);
            callback = SceneActorControl_OnIdle;
            kind = 16;
            actor->field2B2 = one;
            actor->field2B0 = zero;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = zero;
            {
                u32 *flags = &actor->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = zero;
        }
        actor->counter2AC++;
    }
}
