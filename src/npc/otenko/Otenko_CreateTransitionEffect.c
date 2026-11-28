#include "npc/otenko/scene_npc_transition.h"
#include "engine/task.h"

s32 SceneEffect_InitParticleRing(struct TransitionEffect *, const struct ContactPosition *);
s32 Otenko_UpdateTransitionEffect(struct TransitionEffect *);
s32 Otenko_OnTransitionEffectDestroy(struct TransitionEffect *);

struct TransitionEffect *Otenko_CreateTransitionEffect(const struct ContactPosition *origin)
{
    struct TransitionEffect *effect = (struct TransitionEffect *)Task_Create(0xB, sizeof(*effect));
    if (effect) {
        Task_SetCallbacks(&effect->task, (void *)Otenko_UpdateTransitionEffect,
                          (void *)Otenko_OnTransitionEffectDestroy);
        if (SceneEffect_InitParticleRing(effect, origin) < 0) {
            Task_Kill(&effect->task);
            return 0;
        }
    }
    return effect;
}
