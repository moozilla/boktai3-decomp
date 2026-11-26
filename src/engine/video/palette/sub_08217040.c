// COMPILER: old_agbcc
#include "global.h"
#include "engine/video/render.h"
/* Video_SaveDisplayControl: Saves the DISPCNT register for later restoration */

void Video_SaveDisplayControl(void)
{
    gSavedDispCnt = *(vu16 *)0x04000000;
}
