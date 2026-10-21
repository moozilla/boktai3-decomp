#include "directional_particles.h"
s32 DirectionalParticle_Update(void *unused, struct DirectionalParticle *particle)
{
    struct DirectionalParticleMotion *motion = &particle->motion;
    struct ContactPosition *position = &particle->position;
    position->x = (u16)position->x + (u16)motion->velocity.x;
    position->y = (u16)position->y + (u16)motion->velocity.y;
    position->z = (u16)position->z + (u16)motion->velocity.z;
    if (particle->age >= motion->lifetime)
        return 1;
    return 0;
}
