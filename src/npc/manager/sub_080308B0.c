// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* MessageWindow_ResetInstance: Clears the message window singleton pointer used by MessageWindow_OpenFromScript */

void MessageWindow_ResetInstance(void)
{
    gMessageWindow = 0;
}
