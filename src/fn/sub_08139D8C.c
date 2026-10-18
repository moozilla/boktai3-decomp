#include "global.h"

void ParticleBurstEffect_SetStateCallback(void *, void (*)(void));
void sub_081395E0(void);

void ParticleBurstEffect_OnAnimationComplete(void *p)
{
    ParticleBurstEffect_SetStateCallback(p, sub_081395E0);
}
