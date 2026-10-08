#include "global.h"

extern u32 gUnk_086125F0[];
void sub_08249240(u32, u8 *, u32);

void sub_081D18D8(u32 a, u8 *b)
{
    sub_08249240(a, b, gUnk_086125F0[b[4]]);
}
