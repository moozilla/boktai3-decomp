// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* ScanlineWave_Reset: Clears the gScanlineWave active-wave pointer (same global ScanlineWave_Create checks/ScanlineWave_Initialize sets), resetting the scanline wave single */
#define ScanlineWave_Reset sub_08017264

void sub_08017264(void)
{
    gScanlineWave = 0;
}
