#include "global.h"
void DirectionBurst_Emit(const void *, u32, u32, u32);
void sub_0819F49C(const void *origin, s32 angle, s32 spread, u32 kind, u32 parameter)
{
    s32 i;
    u32 k = kind;
    s32 a = angle - spread;
    u32 mask = 0xff;
    for (i = 2; i >= 0; i--) {
        DirectionBurst_Emit(origin, a & mask, k, parameter);
        k += 2;
        a += spread;
    }
}