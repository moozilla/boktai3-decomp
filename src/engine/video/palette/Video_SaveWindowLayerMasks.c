// COMPILER: old_agbcc
#include "global.h"
#include "gba/io_reg.h"
#include "engine/video/render.h"
/* Video_SaveWindowLayerMasks: Saves the WININ and WINOUT registers for later restoration */

void Video_SaveWindowLayerMasks(void)
{
    gSavedWinIn = REG_WININ;
    gSavedWinOut = REG_WINOUT;
}
