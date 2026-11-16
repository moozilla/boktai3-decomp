#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
void SceneActorControl_UpdatePersistentAnimation(struct SceneActorRegistryResetView *);
void SceneActorControl_UpdatePersistentPhase(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *control = (void *)SceneActorControl_UpdatePersistentAnimation;
        u32 kind;
        kind = 34;
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
        ActorShadow_Disable((u8 *)actor + 0x320);
    }
    if (!((u8(*)(struct SceneActorRegistryResetView *)) * (void **)((u8 *)actor + 0x270))(actor))
        ++actor->base.counter2AC;
}
