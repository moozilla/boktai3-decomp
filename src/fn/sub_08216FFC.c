// COMPILER: old_agbcc
#include "global.h"
#include "render.h"
/* Video_SaveWindowLayerMasks: Saves the WININ and WINOUT registers for later restoration */
#define Video_SaveWindowLayerMasks sub_08216FFC

void sub_08216FFC(void)
{
    gSavedWinIn = *(vu16 *)0x04000048;
    gSavedWinOut = *(vu16 *)0x0400004A;
}
