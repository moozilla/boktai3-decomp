#include "global.h"

void DamageNumberDisplay_Show(u8 *, u32, u32, u8 *, u32, u32);
void sub_0802D020(u8 *p, u32 a, u32 b, u32 c)
{
    DamageNumberDisplay_Show(p + 0xc0, a, b, p + 0xc, c, 1);
}
