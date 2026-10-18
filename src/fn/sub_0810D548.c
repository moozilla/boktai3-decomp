// COMPILER: old_agbcc
#include "global.h"
/* Registry30AC_ResetInstance: Clears the singleton task pointer for registry entry 30AC */
#define Registry30AC_ResetInstance sub_0810D548

extern u32 gUnk_020001C8;

void sub_0810D548(void)
{
    gUnk_020001C8 = 0;
}
