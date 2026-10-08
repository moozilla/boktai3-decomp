#include "global.h"

void sub_0813B8A4(u8 *);

void sub_081412A4(u8 *p)
{
    u16 *q;

    sub_0813B8A4(p);
    q = (u16 *)(p + 0x442);
    *q = *q + 1;
}
