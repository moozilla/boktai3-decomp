#include "global.h"
void Collider_SetContactCallback(u8 *p, u32 a, u32 b)
{
    *(u32 *)(p + 0x38) = b;
    *(u32 *)(p + 0x3c) = a;
}
