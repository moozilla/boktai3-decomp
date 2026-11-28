#include "scene/actor/scene_actor_effects.h"

void EffectGroup_FadeFirstActive(void *);

void SceneActorEffects_AdvanceSteps(struct SceneActorEffectsView *actor, s32 count)
{
    if (actor->effect.kind == 1 && count > 0) {
        s32 i = count;
        do {
            EffectGroup_FadeFirstActive(&actor->effect);
        } while (--i != 0);
    }
}
