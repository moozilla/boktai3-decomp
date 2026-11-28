#include "global.h"
#include "engine/ram.h"
#include "player/effects/player_attack_particles.h"

void PlayerAttackEffect_SetEmissionState(u32 state)
{
    if (gPlayerAttackEffect != NULL) {
        if (((struct AttackParticleEffect *)gPlayerAttackEffect)->previous != state)
            ((struct AttackParticleEffect *)gPlayerAttackEffect)->frame = 0;
        ((struct AttackParticleEffect *)gPlayerAttackEffect)->current = state;
    }
}
