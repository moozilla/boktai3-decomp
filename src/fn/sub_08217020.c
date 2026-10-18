// COMPILER: old_agbcc
#include "global.h"
/* Video_RestoreWindowLayerMasks: Restores the saved WININ and WINOUT registers */
#define Video_RestoreWindowLayerMasks sub_08217020

extern u16 gUnk_03001556, gUnk_03001558;

void sub_08217020(void)
{
    vu16 *win = (vu16 *)0x04000048;
    *win = gUnk_03001556;
    win++;
    *win = gUnk_03001558;
}
