#include "bosses/ratatosk/ratatosk_particle.h"

void Ratatosk_ConfigureAnimatedParticle(struct RatatoskParticleConfig *config, u16 lifespan,
                                        u32 speed, u8 angle, u32 behavior, u32 acceleration,
                                        u32 initialSpeed)
{
    config->lifespan = lifespan;
    config->speed = speed;
    config->angle = angle;
    config->behavior = behavior;
    config->fallAcceleration = acceleration;
    config->initialFallSpeed = initialSpeed;
}
