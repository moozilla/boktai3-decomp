#include "global.h"

void Kuro_UpdateBehavior(u32, u32);

s32 Kuro_OnBehaviorUpdate(u32 a, u32 b)
{
    Kuro_UpdateBehavior(a, b);
    return 0;
}
