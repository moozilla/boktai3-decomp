// COMPILER: old_agbcc
#include "global.h"

u32 sub_082215BC(u16 angle)
{
    return ((((angle + 16) & 0xff) >> 5) + 3) & 7;
}
