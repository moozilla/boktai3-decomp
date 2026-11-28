#include "npc/otenko/scene_npc_transition.h"

s32 Otenko_IsTransitionEffectFinished(struct TransitionEffect *effect)
{
    return effect->state == 4;
}
