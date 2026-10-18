// COMPILER: old_agbcc
#include "global.h"
/* Registry3C9F_ResetInstance: Clears the singleton task pointer for registry entry 3C9F */
#define Registry3C9F_ResetInstance sub_0810E91C

extern u32 gUnk_020001CC;

void sub_0810E91C(void)
{
    gUnk_020001CC = 0;
}
