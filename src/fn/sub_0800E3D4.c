// COMPILER: old_agbcc
#include "global.h"
#include "status_screen.h"
/* StatusScreen_ClearActive: Clears gStatusScreen (the live status screen pointer), the same global StatusScreen_Destroy/Initialize set. */
#define StatusScreen_ClearActive sub_0800E3D4

void sub_0800E3D4(void)
{
    gStatusScreen = 0;
}
