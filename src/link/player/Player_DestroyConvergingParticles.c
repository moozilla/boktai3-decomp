#include "global.h"
#include "effects/orbits/homing_effect.h"
#include "player/player.h"

void Player_DestroyConvergingParticles(struct Player *p)
{
    s32 i;

    for (i = 0; i < 4; i++)
        Effect_Remove(&p->convergingEffects.particles[i]);
}
