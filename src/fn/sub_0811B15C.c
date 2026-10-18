#include "global.h"
void Collider_SetExtents(u8 *p, u16 a, u16 b, u16 c)
{
    *(u16 *)(p + 0x18) = a;
    *(u16 *)(p + 0x1a) = b;
    *(u16 *)(p + 0x1c) = c;
}
