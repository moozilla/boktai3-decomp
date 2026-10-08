#include "global.h"
void sub_081D70C4(u8 *, u32);
void sub_081D71A0(u8 *p, s16 *lim)
{
    s16 *c = (s16 *)(p + 0x322);
    (*c)++;
    if (*c >= *lim) sub_081D70C4(p, 2);
}
