// COMPILER: old_agbcc
#include "global.h"
#include "area_effect.h"
/* AreaEffect_ResetSingleton: Clears the global area effect manager pointer. */
#define AreaEffect_ResetSingleton sub_0804DF40

void sub_0804DF40(void)
{
    gAreaEffectManager = 0;
}
