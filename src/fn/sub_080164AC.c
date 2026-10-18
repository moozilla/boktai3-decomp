// COMPILER: old_agbcc
#include "global.h"
/* PaletteLevels_ClearController: Zeroes gUnk_02000070, the active PaletteLevels controller pointer also set by PaletteLevels_Initialize and read by PaletteLevels_Create/PaletteLevels_Configure/PaletteLevels_Reset, so this */
#define PaletteLevels_ClearController sub_080164AC

extern u32 gUnk_02000070;

void sub_080164AC(void)
{
    gUnk_02000070 = 0;
}
