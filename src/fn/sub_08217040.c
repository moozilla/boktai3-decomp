// COMPILER: old_agbcc
#include "global.h"
/* Video_SaveDisplayControl: Saves the DISPCNT register for later restoration */
#define Video_SaveDisplayControl sub_08217040

extern u16 gUnk_0300154C;

void sub_08217040(void)
{
    gUnk_0300154C = *(vu16 *)0x04000000;
}
