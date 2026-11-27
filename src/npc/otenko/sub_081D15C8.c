#include "npc/otenko/scene_npc_transition.h"

void Sprite_Remove(u8 *);
void Effect_Remove(struct TransitionParticle *);

s32 Otenko_OnTransitionEffectDestroy(struct TransitionEffect *effect)
{
    struct TransitionParticle *e;
    s32 i;
    Sprite_Remove((u8 *)&effect->sprite);
    e = effect->particles;
    for (i = 7; i >= 0; i--) {
        Effect_Remove(e);
        e++;
    }
    return 0;
}
