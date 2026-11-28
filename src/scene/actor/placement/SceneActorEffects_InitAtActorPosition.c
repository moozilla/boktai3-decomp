#include "scene/actor/scene_actor_effects.h"

void sub_08232264(void *, struct ContactPosition *, u32, u32, u32);

void SceneActorEffects_InitAtActorPosition(struct ChaseActor *actor)
{
    sub_08232264(&((struct SceneActorEffectsView *)actor)->effect, &actor->base.body.position, 0, 0,
                 0);
}
