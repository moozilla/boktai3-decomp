// COMPILER: old_agbcc
#include "global.h"
#include "palette_level_transitions.h"
/* PaletteLevels_ClearController: Zeroes gPaletteLevels, the active PaletteLevels controller pointer also set by PaletteLevels_Initialize and read by PaletteLevels_Create/PaletteLevels_Configure/PaletteLevels_Reset, so this */
#define PaletteLevels_ClearController sub_080164AC

void sub_080164AC(void)
{
    gPaletteLevels = 0;
}
