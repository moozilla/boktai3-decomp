// COMPILER: old_agbcc
#include "global.h"
/* RegistryF246_ResetInstance: Clears the singleton pointer */
#define RegistryF246_ResetInstance sub_0810CB7C

extern u32 gUnk_020001C4;

void sub_0810CB7C(void)
{
    gUnk_020001C4 = 0;
}
