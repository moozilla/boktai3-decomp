#include "player/player.h"

void Player_OnDirectionalEffectAnimationEvent(struct DirectionalSpriteEffect *effect)
{
    if (effect->active)
        effect->active = 0;
    else
        effect->sprite.flags |= 1;
}
