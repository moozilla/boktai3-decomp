// COMPILER: old_agbcc
#include "global.h"
#include "ui/status_screen/status_screen.h"
/* StatusScreen_ClearActive: Clears gStatusScreen (the live status screen pointer), the same global StatusScreen_Destroy/Initialize set. */

void StatusScreen_ClearActive(void)
{
    gStatusScreen = 0;
}
