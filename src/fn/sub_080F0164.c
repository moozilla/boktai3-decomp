#define SceneActorControl_HideAndDisableCollision sub_080F0164
#include "global.h"
#include "scene_actor_control_phase.h"
void SceneActorTerrain_DisableCollision(void *);
static inline void Hide(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 1;
    if (!actor->drawableKind) {
        u32 *flags = actor->drawable;
        *flags |= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
        tile->flags |= mask;
    }
}
void sub_080F0164(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        Hide(actor);
        {
            u32 state = 3;
            u8 *field = &actor->state2A8;
            u32 zero = 0;
            *field = state;
            actor->phase2A9 = zero;
            actor->behavior2AA = zero;
            actor->counter2AC = zero;
            actor->field2B1 = 1;
        }
        SceneActorTerrain_DisableCollision(actor);
    }
    actor->counter2AC++;
}
