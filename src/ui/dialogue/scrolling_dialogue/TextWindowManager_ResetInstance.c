// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* TextWindowManager_ResetInstance: Clears the text window manager singleton pointer */

void TextWindowManager_ResetInstance(void)
{
    gWindowManager = 0;
}
