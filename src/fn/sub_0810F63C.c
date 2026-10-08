#include "global.h"
extern u32 gUnk_020001D0;
void sub_0810F238(void *);
void sub_0810EE70(void *);
void sub_0811CD08(void *);
s32 sub_0810F63C(u8 *s)
{
    s32 i;
    u8 *p;
    u32 z;
    sub_0810F238(s);
    sub_0810EE70(s);
    p = s + 0xa30;
    i = 1;
    do {
        sub_0811CD08(p);
        p += 0x850;
    } while (--i >= 0);
    z = 0;
    gUnk_020001D0 = z;
    return 1;
}
