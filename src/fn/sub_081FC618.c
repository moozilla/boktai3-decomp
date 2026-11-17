#include "ratatosk_particle.h"

void Ratatosk_SetAnimatedParticleUpdateCallback(struct RatatoskParticleConfig *config,
                                                RatatoskParticleUpdate callback)
{
    config->callback = callback;
}
