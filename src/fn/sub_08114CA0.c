// COMPILER: old_agbcc
#include "global.h"
/* NativeEffect_ResetManager: Clears the native effect singleton pointer */
#define NativeEffect_ResetManager sub_08114CA0

extern u32 gUnk_020004C8;

void sub_08114CA0(void)
{
    gUnk_020004C8 = 0;
}
