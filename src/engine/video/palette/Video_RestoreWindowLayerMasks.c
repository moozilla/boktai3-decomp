// COMPILER: old_agbcc
#include "global.h"
#include "engine/video/render.h"
/* Video_RestoreWindowLayerMasks: Restores the saved WININ and WINOUT registers */

void Video_RestoreWindowLayerMasks(void)
{
    vu16 *win = (vu16 *)0x04000048;
    *win = gSavedWinIn;
    win++;
    *win = gSavedWinOut;
}
