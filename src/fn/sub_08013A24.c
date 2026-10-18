#include "global.h"
u32 ScaledShadow_Initialize(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void ScaledShadow_SetGraphic(u32, u32, u32);
u32 ScaledShadow_InitializeDefaultSmall(u32 a, u32 b)
{
    u32 r = ScaledShadow_Initialize(a, b, 0x40, 0x10, 0x40, 0, 0xa00, 1, 5, 1, 0);
    if (r == 0)
        ScaledShadow_SetGraphic(a, 0x1c1b, 6);
    return r;
}
