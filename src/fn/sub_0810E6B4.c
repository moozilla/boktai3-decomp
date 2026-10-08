#include "global.h"

extern u32 gUnk_020001CC;
void sub_0810DA38(u8 *);
void sub_0810DE6C(u8 *);
void sub_0811CD08(u8 *);

s32 sub_0810E6B4(u8 *p)
{
    sub_0810DA38(p);
    sub_0810DE6C(p);
    sub_0811CD08(p + 0xa8c);
    gUnk_020001CC = 0;
    return 1;
}
