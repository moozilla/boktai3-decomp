#include "player/effects/orbit_effect.h"

s32 PlayerBurst_Destroy(struct PlayerOrbitEffect *effect)
{
    s32 i;
    Anim_Remove(&effect->drawable);
    for (i = 0; i < 16; i++)
        Effect_Remove(&effect->particles[i]);
    return 0;
}
