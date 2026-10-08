#include "global.h"

extern u32 gUnk_08612160[];
void sub_08249240(u32, u8 *, u32);

void sub_081C66E0(u32 a, u8 *b)
{
    sub_08249240(a, b, gUnk_08612160[b[4]]);
}
