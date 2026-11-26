// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* TextWindowManager_ResetInstance: Clears the text window manager singleton pointer */
#define TextWindowManager_ResetInstance sub_080335D4

void sub_080335D4(void)
{
    gWindowManager = 0;
}
