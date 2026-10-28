// COMPILER: old_agbcc
#include "global.h"
#include "render.h"
/* Video_RestoreDisplayControl: Restores the saved DISPCNT register */
#define Video_RestoreDisplayControl sub_08217050

void sub_08217050(void)
{
    *(vu16 *)0x04000000 = gSavedDispCnt;
}
