#include "global.h"

u8 *sub_0821A2C4(u32);
void sub_08217EAC(u8 *);
void sub_0803F5EC(u8 *, u8 *);

u32 sub_0803F7D8(u8 *p)
{
    u8 *q = sub_0821A2C4(0xF0DE);
    sub_08217EAC(p + 0xc);
    if (q != 0)
        sub_0803F5EC(q, p);
    return 0;
}
