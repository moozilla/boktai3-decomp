// COMPILER: old_agbcc
#include "global.h"
#include "engine/video/render.h"
/* Video_RestoreDisplayControl: Restores the saved DISPCNT register */

void Video_RestoreDisplayControl(void)
{
    *(vu16 *)0x04000000 = gSavedDispCnt;
}
