// COMPILER: old_agbcc
#include "global.h"
/* Video_SaveWindowLayerMasks: Saves the WININ and WINOUT registers for later restoration */
#define Video_SaveWindowLayerMasks sub_08216FFC

extern u16 gUnk_03001556, gUnk_03001558;

void sub_08216FFC(void)
{
    gUnk_03001556 = *(vu16 *)0x04000048;
    gUnk_03001558 = *(vu16 *)0x0400004A;
}
