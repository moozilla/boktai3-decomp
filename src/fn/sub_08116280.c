// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* SpriteDrop_ClearManager: Clears the singleton sprite drop manager pointer */
#define SpriteDrop_ClearManager sub_08116280

void sub_08116280(void)
{
    gCompactParticlePool = 0;
}
