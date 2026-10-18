#include "global.h"

void Vector3_CopyComponents(u8 *d, u8 *s)
{
    *(u32 *)(d + 0) = *(u32 *)(s + 0);
    *(u32 *)(d + 4) = *(u32 *)(s + 4);
    *(u32 *)(d + 8) = *(u32 *)(s + 8);
}
