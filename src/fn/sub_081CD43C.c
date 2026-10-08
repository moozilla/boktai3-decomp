#include "global.h"

extern u32 gUnk_08612344[];
void sub_08249240(u32, u8 *, u32);

void sub_081CD43C(u32 a, u8 *b)
{
    sub_08249240(a, b, gUnk_08612344[b[4]]);
}
