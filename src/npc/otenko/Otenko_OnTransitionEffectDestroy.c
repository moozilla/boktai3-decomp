#include "npc/otenko/scene_npc_transition.h"

void Sprite_Remove(u8 *);
void Effect_Remove(struct TransitionParticle *);

s32 Otenko_OnTransitionEffectDestroy(struct TransitionEffect *effect)
{
    s32 i;
    Sprite_Remove((u8 *)&effect->sprite);
    for (i = 0; i < 8; i++)
        Effect_Remove(&effect->particles[i]);
    return 0;
}
