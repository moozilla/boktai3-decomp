#include "global.h"

void Memory_Clear(u32, u32);

void sub_08218CB0(void)
{
    Memory_Clear(0x02034400, 0x800);
}
