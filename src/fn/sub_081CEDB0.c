#include "global.h"

extern u32 gUnk_086124D4[];
void sub_08249240(u32, u8 *, u32);

void sub_081CEDB0(u32 a, u8 *b)
{
    sub_08249240(a, b, gUnk_086124D4[b[4]]);
}
