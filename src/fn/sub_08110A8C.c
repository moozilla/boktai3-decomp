// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* SaveMenu_ClearSingleton: Clears the global menu task pointer */
#define SaveMenu_ClearSingleton sub_08110A8C

void sub_08110A8C(void)
{
    gSaveMenu = 0;
}
