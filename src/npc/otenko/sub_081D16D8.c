#include "npc/otenko/scene_npc_transition.h"

s32 Otenko_IsTransitionEffectFinished(struct TransitionEffect *effect)
{
    s32 r = 0;
    if (effect->state == 4)
        r = 1;
    return r;
}
