// COMPILER: old_agbcc
#include "global.h"
#include "engine/video/render.h"
/* Video_RestoreWindowLayerMasks: Restores the saved WININ and WINOUT registers */
#define Video_RestoreWindowLayerMasks sub_08217020

void sub_08217020(void)
{
    vu16 *win = (vu16 *)0x04000048;
    *win = gSavedWinIn;
    win++;
    *win = gSavedWinOut;
}
