#include "global.h"
#include "engine/ram.h"
#include "player/effects/player_attack_particles.h"

void PlayerAttackEffect_SetElementMode(u32 element)
{
    if (gPlayerAttackEffect != NULL)
        ((struct AttackParticleEffect *)gPlayerAttackEffect)->element = element;
}
