#include "global.h"
#include "scene_actor_control_phase.h"

void Bee_ResetReactionAnimation(void);
static inline void ResetAnimation(struct SceneActorRegistryResetView *actor, u32 phase,
                                  u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void Bee_UpdateReactionPhase(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        actor->body.direction = actor->tracking.heading;
        {
            void *callback = (void *)Bee_ResetReactionAnimation;
            u32 kind = 0x25;
            actor->field2B2 = 1;
            actor->field2B0 = 0;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = 0;
            {
                u32 *flags = &actor->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = 0;
        }
    }
    if ((s32)actor->counter2AC > 0x28 && actor->field2B3)
        ResetAnimation(actor, 2, 3);
    actor->counter2AC++;
}
