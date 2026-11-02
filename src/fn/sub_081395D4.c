#include "particle_effect_state.h"

void ParticleBurstEffect_SetStateCallback(struct BurstEntry *effect,
                                          void (*update)(struct BurstEntry *))
{
    effect->update = update;
    effect->entered = 1;
}
