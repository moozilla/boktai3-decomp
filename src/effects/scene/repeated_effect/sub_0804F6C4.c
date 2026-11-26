// COMPILER: old_agbcc
#include "global.h"
#include "effects/scene/repeated_effect.h"
/* RepeatedEffect_ResetManager: Clears the repeated effect manager singleton pointer; preserves the existing file-local provisional name. */

void RepeatedEffect_ResetManager(void)
{
    gRepeatedEffectManager = 0;
}
