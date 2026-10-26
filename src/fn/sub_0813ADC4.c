// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* SpriteParticle_ResetManager: clears the particle manager singleton pointer */
#define SpriteParticle_ResetManager sub_0813ADC4

void sub_0813ADC4(void)
{
    gScreenMoteManager = 0;
}
