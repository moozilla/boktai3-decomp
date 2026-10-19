#include "global.h"

void ParticleBurstEffect_SetStateCallback(void *, void (*)(void));
void SpriteEffect_HideWithScatterBurst(void);

void ParticleBurstEffect_OnAnimationComplete(void *p)
{
    ParticleBurstEffect_SetStateCallback(p, SpriteEffect_HideWithScatterBurst);
}
