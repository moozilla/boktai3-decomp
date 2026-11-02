#include "particle_effect_state.h"

void FollowingSpriteEffect_SetStateCallback(struct FollowingEffect *effect,
                                            void (*update)(struct FollowingEffect *))
{
    effect->update = update;
    effect->entered = 1;
}
