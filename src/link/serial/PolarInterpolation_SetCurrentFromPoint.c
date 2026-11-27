#include "global.h"

void Math_GetTargetAngleAndDistance(u8 *, u32, u8 *, u16 *);

void PolarInterpolation_SetCurrentFromPoint(u8 *p, u32 q)
{
    u8 a;
    u16 b;
    Math_GetTargetAngleAndDistance(p, q, &a, &b);
    *(u32 *)(p + 4) = a << 8;
    *(u32 *)(p + 0x10) = b << 8;
}
