// COMPILER: old_agbcc
#include "global.h"
/* ScreenOrbitEffect_ResetSingleton: Clears the four-slot screen effect singleton pointer; orbit interpretation is tentative */
#define ScreenOrbitEffect_ResetSingleton sub_082329C8

extern u32 gUnk_02000098;

void sub_082329C8(void)
{
    gUnk_02000098 = 0;
}
