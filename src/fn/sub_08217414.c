// COMPILER: old_agbcc
#include "global.h"
/* Palette_LoadWorkingColors: Copies 48 colors into the shared working palette buffer using DMA */
#define Palette_LoadWorkingColors sub_08217414

void sub_08217414(const void *src)
{
    vu32 *dma = (vu32 *)0x040000D4;
    dma[0] = (u32)src;
    dma[1] = 0x03005160;
    dma[2] = 0x84000018;
    dma[2];
}
