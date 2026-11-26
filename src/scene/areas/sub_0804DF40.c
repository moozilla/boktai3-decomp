// COMPILER: old_agbcc
#include "global.h"
#include "scene/areas/area_effect.h"
/* AreaEffect_ResetSingleton: Clears the global area effect manager pointer. */

void AreaEffect_ResetSingleton(void)
{
    gAreaEffectManager = 0;
}
