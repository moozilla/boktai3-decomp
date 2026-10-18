#include "global.h"

void PlayerClone_SetLifetimeAndBehavior(u8 *p, u16 a, u32 b, u8 c)
{
    *(u16 *)(p + 2) = a;
    *(u32 *)(p + 0x8) = b;
    *(u8 *)(p + 6) = c;
}
