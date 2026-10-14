#include "global.h"

void sub_08045D20(u8 *p, u32 mask)
{
    *(u16 *)(p + 4) &= ~mask;
}
