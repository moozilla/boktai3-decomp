// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* NativeEffect_ResetManager: Clears the native effect singleton pointer */
#define NativeEffect_ResetManager sub_08114CA0

void sub_08114CA0(void)
{
    gSceneCallbackPool = 0;
}
