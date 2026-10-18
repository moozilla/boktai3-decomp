#include "global.h"

extern u32 gUnk_08612160[];
void sub_08249240(u32, u8 *, u32);

void Actor_DispatchPrimaryState(u32 a, u8 *b)
{
    sub_08249240(a, b, gUnk_08612160[b[4]]);
}
