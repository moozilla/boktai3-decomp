#include "global.h"
u32 ScaledShadow_Initialize(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

u32 sub_08013AB4(u32 a, u32 b, u32 c, u32 d)
{
    return ScaledShadow_Initialize(a, b, d, c, d, 0, 0xa00, 1, 5, 1, 0);
}
