// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* ScreenOrbitEffect_ResetSingleton: Clears the four-slot screen effect singleton pointer; orbit interpretation is tentative */
#define ScreenOrbitEffect_ResetSingleton sub_082329C8

void sub_082329C8(void)
{
    gEffectQuad = 0;
}
