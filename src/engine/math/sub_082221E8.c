#include "global.h"

void Math_SetMotionPosition(u8 *a, u32 *b)
{
    u32 y = b[1];
    u32 x = b[0];
    *(u32 *)(a + 0x20) = x;
    *(u32 *)(a + 0x24) = y;
}
