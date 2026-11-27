// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* ScreenOrbitEffect_ResetSingleton: Clears the four-slot screen effect singleton pointer; orbit interpretation is tentative */

void ScreenOrbitEffect_ResetSingleton(void)
{
    gEffectQuad = 0;
}
