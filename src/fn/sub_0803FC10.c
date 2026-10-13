#include "global.h"

void sub_0811E48C(u16 *, s32);

void sub_0803FC10(s16 *p)
{
    u16 buf[8];
    sub_0811E48C(buf, p[0xe]);
    p[0] = buf[3];
    p[1] = buf[2];
}
