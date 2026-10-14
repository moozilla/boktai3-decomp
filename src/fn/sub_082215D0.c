// COMPILER: old_agbcc
#include "global.h"

u32 sub_082215D0(u16 angle)
{
    return ((((angle + 32) & 0xff) >> 6) + 1) & 3;
}
