// COMPILER: old_agbcc
#include "global.h"
#include "gba/io_reg.h"
/* Palette_LoadWorkingColors: Copies 48 colors into the shared working palette buffer using DMA */

void Palette_LoadWorkingColors(const void *src)
{
    DmaCopy32(3, src, 0x03005160, 48 * sizeof(u16));
}
