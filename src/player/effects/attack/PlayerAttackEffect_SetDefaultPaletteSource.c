#include "global.h"
#include "engine/ram.h"
#include "player/effects/player_attack_particles.h"

void PlayerAttackEffect_SetDefaultPaletteSource(u32 palette)
{
    struct AttackParticleEffect *effect = (struct AttackParticleEffect *)gPlayerAttackEffect;
    if (effect != NULL) {
        if (effect->element == 0)
            effect->graphics[0].palette = (void *)palette;
    }
}
