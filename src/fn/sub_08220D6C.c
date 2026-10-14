// COMPILER: old_agbcc
#include "global.h"

void sub_08220D6C(u8 *p)
{
    p[7] = p[6];
    *(u16 *)(p + 0xc) = 64;
}
