#include "global.h"
void sub_08076034(u8 *, u32);
void sub_080A21A4(u8 *p)
{
    if (p[0x2b2] != 0) p[0x2b2] = 0;
    sub_08076034(p, 1);
}
