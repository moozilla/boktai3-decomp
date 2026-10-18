#include "global.h"

void SpriteParticles_UpdateEntries(void);

s32 ParticlePool_OnUpdate(void)
{
    SpriteParticles_UpdateEntries();
    return 0;
}
