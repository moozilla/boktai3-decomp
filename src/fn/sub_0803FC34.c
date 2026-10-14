#include "global.h"

void sub_0811E5D0(u8 *, s32);

void sub_0803FC34(s16 *p)
{
    u8 buf[8];
    u8 *b = buf;
    sub_0811E5D0(buf, p[0xf]);
    p[2] = b[1];
    p[3] = b[2];
    p[4] = *(u16 *)(b + 4);
    p[6] = b[3];
}
