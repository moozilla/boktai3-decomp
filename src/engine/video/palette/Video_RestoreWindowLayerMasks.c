// COMPILER: old_agbcc
#include "global.h"
#include "gba/io_reg.h"
#include "engine/video/render.h"
/* Video_RestoreWindowLayerMasks: Restores the saved WININ and WINOUT registers */

void Video_RestoreWindowLayerMasks(void)
{
    REG_WININ = gSavedWinIn;
    REG_WINOUT = gSavedWinOut;
}
