// COMPILER: old_agbcc
#include "global.h"
/* CircleWindow_InitManager: Zeroes gUnk_0200009C, the same global that CircleWindow_Initialize assigns and CircleWindow_Destroy clears to 0; a module-level reset distinct from th */
#define CircleWindow_InitManager sub_08018F2C

extern u32 gUnk_0200009C;

void sub_08018F2C(void)
{
    gUnk_0200009C = 0;
}
