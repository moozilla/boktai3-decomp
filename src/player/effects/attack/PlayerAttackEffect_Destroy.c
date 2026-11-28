#include "global.h"
#include "engine/ram.h"
#include "player/effects/player_attack_particles.h"

s32 Sprite_Remove(struct DisplaySequenceSprite *);

s32 PlayerAttackEffect_Destroy(struct AttackParticleEffect *effect)
{
    s32 i;
    for (i = 0; i < 16; i++)
        Sprite_Remove(&effect->sprites[i]);
    gPlayerAttackEffect = NULL;
}
