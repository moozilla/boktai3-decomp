#include "global.h"

void Memory_Clear(u32, u32);

void Clock_ClearState(void)
{
    Memory_Clear(0x03005430, 0x24);
}
