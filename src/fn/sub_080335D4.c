// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* TextWindowManager_ResetInstance: Clears the text window manager singleton pointer */
#define TextWindowManager_ResetInstance sub_080335D4

void sub_080335D4(void)
{
    gWindowManager = 0;
}
