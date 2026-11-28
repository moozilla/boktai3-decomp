#include "global.h"
#include "bosses/ratatosk/particle_config_layout.h"

void Ratatosk_ConfigureParticle(struct ParticleConfig *config, u16 duration, u32 speed, u8 angle,
                                u32 kind, u32 acceleration)
{
    config->duration = duration;
    config->mode = speed;
    config->angle = angle;
    config->kind = kind;
    config->scale = acceleration;
}
