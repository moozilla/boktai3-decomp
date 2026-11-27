// COMPILER: old_agbcc
#include "global.h"
#include "engine/video/render.h"
/* Video_SaveWindowLayerMasks: Saves the WININ and WINOUT registers for later restoration */

void Video_SaveWindowLayerMasks(void)
{
    gSavedWinIn = *(vu16 *)0x04000048;
    gSavedWinOut = *(vu16 *)0x0400004A;
}
