// MATCHING_CONCESSION: register-pinning
#include "scene_actor_restoration.h"
void SceneActorReaction_RestoreAfterDelay(struct SceneActorRegistryActor *actor, s32 frame)
{
    struct SceneActorRegistryActor *owner = actor;

    if (((struct SceneActorRestorationState *)owner)->threshold <= frame) {
        const void *control = ((struct SceneActorRestorationState *)owner)->control;
        u32 kind = 14;
        u32 zero;
        u32 one;
        {
            u8 *field = &owner->field2B2;
            zero = 0;
            one = 1;
            *field = one;
        }
        owner->field2B0 = zero;
        *((u8 *)owner + 0x2B5) = kind;
        owner->control = control;
        owner->field2C8 = zero;
        {
            u32 offset = 0x114;
            u32 *field;
            register u32 mask asm("r1");
            field = (void *)((u8 *)owner + offset);
            asm("" : "+r"(field));
            mask = ~1;
            *field &= mask;
            {
                u32 *clear = &owner->field104;
                *clear = zero;
            }
        }
        {
            u32 state = ((struct SceneActorRestorationState *)owner)->state,
                phase = ((struct SceneActorRestorationState *)owner)->phase,
                behavior = ((struct SceneActorRestorationState *)owner)->behavior;
            owner->state2A8 = state;
            owner->phase2A9 = phase;
            owner->behavior2AA = behavior;
        }
        owner->counter2AC = zero;
        owner->field2B1 = one;
        sub_0812E084((u8 *)owner + 0x460, gUnk_0824E128, 0x74);
    }
}
