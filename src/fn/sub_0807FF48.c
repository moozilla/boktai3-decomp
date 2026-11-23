#include "scene_actor_restoration.h"
void SceneActorReaction_RestoreAlternateAfterDelay(struct ChaseActor *actor, s32 frame)
{
    struct ChaseActor *owner = actor;

    if (((struct SceneActorRestorationState *)owner)->threshold <= frame) {
        const void *control = ((struct SceneActorRestorationState *)owner)->control;
        u32 kind = 14;
        u32 zero;
        u32 one;
        {
            u8 *field = &owner->base.field2B2;
            zero = 0;
            one = 1;
            *field = one;
        }
        owner->base.stepDone = zero;
        *((u8 *)owner + 0x2B5) = kind;
        owner->base.target = control;
        owner->base.field2C8 = zero;
        {
            u32 *field;
            u32 mask;
            field = &owner->base.pendingControlFlags;
            mask = ~1;
            *field &= mask;
            {
                u32 *clear = &owner->base.pendingControl;
                *clear = zero;
            }
        }
        {
            u32 state = ((struct SceneActorRestorationState *)owner)->state,
                phase = ((struct SceneActorRestorationState *)owner)->phase,
                behavior = ((struct SceneActorRestorationState *)owner)->behavior;
            owner->base.state2A8 = state;
            owner->base.phase2A9 = phase;
            owner->base.behavior2AA = behavior;
        }
        owner->base.counter2AC = zero;
        owner->base.field2B1 = one;
        SceneActorReaction_ResetResponseState((u8 *)owner + 0x460, gUnk_0824E128, 0xA0);
    }
}
