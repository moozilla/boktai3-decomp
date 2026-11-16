#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"
void SceneActor_EndReactionAnimation(void *);
void SceneActorReaction_RestoreAlternateAfterDelay(struct SceneActorRegistryResetView *);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
void SceneActorReaction_RestoreAfterControl(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SetControl(actor, SceneActor_EndReactionAnimation, 35);
        actor->base.field2B0 = 0;
    }
    if (actor->base.field2B0) {
        struct SceneActorReactionCommand *command = (void *)&actor->group.saved.control;
        void *callback = SceneActorControl_OnIdle;
        u32 one = 1;
        u32 zero;
        u32 state;
        command->duration = 30;
        command->payload = callback;
        zero = 0;
        command->first = zero;
        command->second = zero;
        command->kind = one;
        state = 3;
        actor->base.state2A8 = state;
        actor->base.phase2A9 = zero;
        actor->base.behavior2AA = zero;
        actor->base.counter2AC = zero;
        {
            u16 offset = 0x2B1;
            *((u8 *)actor + offset) = one;
        }
        SetControl(actor, SceneActorReaction_RestoreAlternateAfterDelay, 14);
    }
}
