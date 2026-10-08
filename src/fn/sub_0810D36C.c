#include "global.h"

extern u32 gUnk_020001C8;
void sub_0810D188(u8 *);
void sub_0811CD08(u8 *);
s32 sub_0810D36C(u8 *p)
{
    s32 i;
    u32 z;
    sub_0810D188(p);
    p += 0x940;
    i = 1;
    do {
        sub_0811CD08(p);
        p += 0x850;
        i--;
    } while (i >= 0);
    z = 0;
    gUnk_020001C8 = z;
    return 1;
}