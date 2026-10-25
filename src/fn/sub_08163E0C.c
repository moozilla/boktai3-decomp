#include "global.h"

void CollisionVolume_InitCollider(void *);
void ParticlePool_InitParticles(void *);

s32 ParticlePool_Init(void *p)
{
    CollisionVolume_InitCollider(p);
    ParticlePool_InitParticles(p);
    return 0;
}
