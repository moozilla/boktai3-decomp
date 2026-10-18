// COMPILER: old_agbcc
#include "global.h"
/* Video_RestoreDisplayControl: Restores the saved DISPCNT register */
#define Video_RestoreDisplayControl sub_08217050

extern u16 gUnk_0300154C;

void sub_08217050(void)
{
    *(vu16 *)0x04000000 = gUnk_0300154C;
}
