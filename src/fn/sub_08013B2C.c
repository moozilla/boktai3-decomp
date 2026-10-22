#include "global.h"
u32 ScaledShadow_Initialize(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void ScaledShadow_SetGraphic(u32, u32, u32);

u32 sub_08013B2C(u32 a, u32 b, u32 c, u32 d)
{
    u32 r = ScaledShadow_Initialize(a, b, d, c, d, 0, 0xa00, 1, 5, 1, 0);
    if (r == 0)
        ScaledShadow_SetGraphic(a, 0x1c1b, 7);
    return r;
}
