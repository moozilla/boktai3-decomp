// COMPILER: old_agbcc
#include "global.h"
/* SpriteParticle_ResetManager: clears the particle manager singleton pointer */
#define SpriteParticle_ResetManager sub_0813ADC4

extern u32 gUnk_020001E0;

void sub_0813ADC4(void)
{
    gUnk_020001E0 = 0;
}
