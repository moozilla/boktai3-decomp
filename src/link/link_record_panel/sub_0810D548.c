// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* Registry30AC_ResetInstance: Clears the singleton task pointer for registry entry 30AC */
#define Registry30AC_ResetInstance sub_0810D548

void sub_0810D548(void)
{
    gPanelSequence = 0;
}
