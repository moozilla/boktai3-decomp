#include "global.h"
void sub_081B95E4(u8 *);
void sub_0821A0C0(u8 *);
void sub_081B6668(u8 *p)
{
    s16 *c;
    sub_081B95E4(p + 0x60);
    c = (s16 *)(p + 0x9a);
    if (--*c == 0) sub_0821A0C0(p);
}
