// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* Registry3C9F_ResetInstance: Clears the singleton task pointer for registry entry 3C9F */
#define Registry3C9F_ResetInstance sub_0810E91C

void sub_0810E91C(void)
{
    gConfirmPanel = 0;
}
