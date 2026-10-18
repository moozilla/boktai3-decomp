#include "global.h"
u32 ScaledShadow_Initialize(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
u32 ScaledShadow_InitializeDefault(u32 a, u32 b)
{
    return ScaledShadow_Initialize(a, b, 0x40, 0x10, 0x40, 0, 0xa00, 1, 5, 1, 0);
}
