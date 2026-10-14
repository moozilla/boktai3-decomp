// COMPILER: old_agbcc
#include "global.h"

void sub_081DACEC(u8 *p, u32 v)
{
    *(u16 *)(p + 0x28) = v;
    *(u16 *)(p + 0x2a) = v;
    *(u16 *)(p + 0x2c) = v;
}
