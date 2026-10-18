// COMPILER: old_agbcc
#include "global.h"
/* ScanlineWave_Reset: Clears the gUnk_0200007C active-wave pointer (same global ScanlineWave_Create checks/ScanlineWave_Initialize sets), resetting the scanline wave single */
#define ScanlineWave_Reset sub_08017264

extern u32 gUnk_0200007C;

void sub_08017264(void)
{
    gUnk_0200007C = 0;
}
