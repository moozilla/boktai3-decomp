#include "global.h"
#include "scene_actor_control_phase.h"
void SceneActorTerrain_DisableCollision(void *);
static inline void Hide(struct ChaseActor *actor)
{
    u32 mask = 1;
    if (!actor->base.drawableKind) {
        u32 *flags = actor->base.drawable;
        *flags |= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->base.drawable + 0x20);
        tile->flags |= mask;
    }
}
void SceneActorControl_HideAndDisableCollision(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        Hide(actor);
        {
            u32 state = 3;
            u8 *field = &actor->base.state2A8;
            u32 zero = 0;
            *field = state;
            actor->base.phase2A9 = zero;
            actor->base.behavior2AA = zero;
            actor->base.counter2AC = zero;
            actor->base.field2B1 = 1;
        }
        SceneActorTerrain_DisableCollision(actor);
    }
    actor->base.counter2AC++;
}
