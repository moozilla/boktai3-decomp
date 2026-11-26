// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/render.h"
/* Video_SaveDisplayControl: Saves the DISPCNT register for later restoration */
#define Video_SaveDisplayControl sub_08217040

void sub_08217040(void)
{
    gSavedDispCnt = *(vu16 *)0x04000000;
}
