#include "global.h"
void Registry1745_SetState(u8 *p, u8 a, u32 b)
{
    u32 z = 0;
    p[0x1c] = a;
    *(u32 *)(p + 0xb0) = b;
    *(u32 *)(p + 0x30) = z;
    p[0x22] = 1;
}
