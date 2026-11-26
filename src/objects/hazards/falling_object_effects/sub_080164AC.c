// COMPILER: old_agbcc
#include "global.h"
#include "effects/palette/palette_level_transitions.h"
/* PaletteLevels_ClearController: Zeroes gPaletteLevels, the active PaletteLevels controller pointer also set by PaletteLevels_Initialize and read by PaletteLevels_Create/PaletteLevels_Configure/PaletteLevels_Reset, so this */

void PaletteLevels_ClearController(void)
{
    gPaletteLevels = 0;
}
