#include "global.h"

void Vector3_AddComponents(u8 *d, u8 *a, u8 *b)
{
    *(u32 *)(d + 0) = *(u32 *)(a + 0) + *(u32 *)(b + 0);
    *(u32 *)(d + 4) = *(u32 *)(a + 4) + *(u32 *)(b + 4);
    *(u32 *)(d + 8) = *(u32 *)(a + 8) + *(u32 *)(b + 8);
}
