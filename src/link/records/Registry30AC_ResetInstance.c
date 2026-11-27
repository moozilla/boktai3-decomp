// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* Registry30AC_ResetInstance: Clears the singleton task pointer for registry entry 30AC */

void Registry30AC_ResetInstance(void)
{
    gPanelSequence = 0;
}
