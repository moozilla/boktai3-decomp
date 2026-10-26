// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* MessageWindow_ResetInstance: Clears the message window singleton pointer used by MessageWindow_OpenFromScript */
#define MessageWindow_ResetInstance sub_080308B0

void sub_080308B0(void)
{
    gMessageWindow = 0;
}
