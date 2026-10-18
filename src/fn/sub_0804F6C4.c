// COMPILER: old_agbcc
#include "global.h"
/* RepeatedEffect_ResetManager: Clears the repeated effect manager singleton pointer; preserves the existing file-local provisional name. */
#define RepeatedEffect_ResetManager sub_0804F6C4
#define RepeatedEffect_ResetManager sub_0804F6C4

extern u32 gUnk_02000110;

void RepeatedEffect_ResetManager(void)
{
    gUnk_02000110 = 0;
}
