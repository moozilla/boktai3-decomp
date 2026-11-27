#include "scene/actor/scene_actor_control_phase.h"
#include "scene/actor/scene_actor_reaction.h"
void SceneActorControl_UpdatePersistentAnimation(struct ChaseActor *);
void SceneActorControl_UpdatePersistentPhase(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *control = (void *)SceneActorControl_UpdatePersistentAnimation;
        u32 kind;
        kind = 34;
        actor->base.field2B2 = 1;
        actor->base.stepDone = 0;
        actor->base.controlKind = kind;
        actor->base.target = control;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = 0;
        ActorShadow_Disable(actor->base.shadow);
    }
    if (!((u8(*)(struct ChaseActor *))actor->base.contactQuery.opaque)(actor))
        ++actor->base.counter2AC;
}
