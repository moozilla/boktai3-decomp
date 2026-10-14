// COMPILER: old_agbcc
#include "global.h"

void sub_08217414(const void *src)
{
    vu32 *dma = (vu32 *)0x040000D4;
    dma[0] = (u32)src;
    dma[1] = 0x03005160;
    dma[2] = 0x84000018;
    dma[2];
}
