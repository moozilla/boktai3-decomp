// COMPILER: old_agbcc
#include "global.h"

void sub_081041F0(u8 *p, u16 mask)
{
    *(u16 *)(p + 0x156) &= ~mask;
}
