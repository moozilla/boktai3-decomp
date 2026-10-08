#include "global.h"
void sub_082156B8(s32);
void sub_08112118(void *);
u32 sub_0811BB1C(void);
void sub_0821AD08(u32, void *);
void sub_081123F0(u8 *s)
{
    u32 loc[3];
    u32 *p;
    u32 *q;
    u32 m;
    sub_082156B8(0);
    sub_082156B8(1);
    sub_082156B8(2);
    sub_082156B8(3);
    sub_08112118(s);
    p = (u32 *)(s + 0x1210);
    if (*p != 0) {
        loc[0] = sub_0811BB1C();
        m = 0xFFFF0000;
        loc[1] = (loc[1] & m) | 1;
        q = &loc[1];
        q[1] = (u32)loc;
        sub_0821AD08(*p, q);
    }
}
