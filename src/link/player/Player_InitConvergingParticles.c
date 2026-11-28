#include "global.h"
#include "player/player.h"
#include "effects/orbits/homing_effect.h"

void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);

void Player_InitConvergingParticles(struct Player *p)
{
    struct ConvergingEffectState *state = &p->convergingEffects;
    s32 i;

    state->graphics = AnimData_FindEntry(0x1C1E);
    for (i = 0; i < 4; i++) {
        struct ConvergingParticle *particle = &state->particles[i];
        ParticleSprite_Initialize(particle, state->graphics, 1);
        Effect_SetOrigin(particle, -4, -4);
        Effect_SetPalette(particle, 1);
        particle->renderMode = 2;
    }
    state->next = 0;
    state->ticks = 0;
}
