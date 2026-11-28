// COMPILER: old_agbcc
#include "global.h"
#include "gba/io_reg.h"
#include "engine/video/render.h"
/* Video_RestoreDisplayControl: Restores the saved DISPCNT register */

void Video_RestoreDisplayControl(void)
{
    REG_DISPCNT = gSavedDispCnt;
}
