// COMPILER: old_agbcc
#include "global.h"

void sub_0813B3AC(u8 *p, u32 bits)
{
    *(u16 *)(p + 0xb34) = bits | *(u16 *)(p + 0xb34);
}
