// MATCHING_CONCESSION: register-pinning
#include "directional_particles.h"
s32 DirectionalParticle_Update(void *unused, struct DirectionalParticle *particle)
{
    struct DirectionalParticleMotion *motion = &particle->motion;
    struct ContactPosition *position = &particle->position;
    {
        register u32 movement asm("r0") = (u16)motion->velocity.x;
        register u32 current asm("r4") = (u16)position->x;
        position->x = movement + current;
    }
    {
        register u32 movement asm("r0") = (u16)motion->velocity.y;
        register u32 current asm("r4") = (u16)position->y;
        position->y = movement + current;
    }
    {
        register u32 movement asm("r0") = (u16)motion->velocity.z;
        register u32 current asm("r4") = (u16)position->z;
        position->z = movement + current;
    }
    if (particle->age >= motion->lifetime)
        return 1;
    return 0;
}
