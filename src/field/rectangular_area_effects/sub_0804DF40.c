// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "field/rectangular_area_effects/area_effect.h"
/* AreaEffect_ResetSingleton: Clears the global area effect manager pointer. */
#define AreaEffect_ResetSingleton sub_0804DF40

void sub_0804DF40(void)
{
    gAreaEffectManager = 0;
}
