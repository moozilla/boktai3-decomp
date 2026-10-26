// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* CircleWindow_InitManager: Zeroes gCircleWindow, the same global that CircleWindow_Initialize assigns and CircleWindow_Destroy clears to 0; a module-level reset distinct from th */
#define CircleWindow_InitManager sub_08018F2C

void sub_08018F2C(void)
{
    gCircleWindow = 0;
}
