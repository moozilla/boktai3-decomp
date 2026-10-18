#include "global.h"

void SunOrb_SetUpdateCallback(u8 *p, u32 a)
{
    *(u32 *)(p + 0x1C) = a;
}
