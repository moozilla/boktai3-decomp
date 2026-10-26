// COMPILER: old_agbcc
#include "global.h"
#include "repeated_effect.h"
/* RepeatedEffect_ResetManager: Clears the repeated effect manager singleton pointer; preserves the existing file-local provisional name. */
#define RepeatedEffect_ResetManager sub_0804F6C4
#define RepeatedEffect_ResetManager sub_0804F6C4

void RepeatedEffect_ResetManager(void)
{
    gRepeatedEffectManager = 0;
}
