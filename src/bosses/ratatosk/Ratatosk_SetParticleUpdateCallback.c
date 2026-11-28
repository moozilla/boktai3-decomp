#include "global.h"
#include "bosses/ratatosk/particle_config_layout.h"

void Ratatosk_SetParticleUpdateCallback(struct ParticleConfig *config, u32 callback)
{
    config->callback = callback;
}
