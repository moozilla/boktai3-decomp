#include "global.h"
void sub_0810FDC0(u8 *);
void sub_0811CD08(u8 *);
extern u32 gUnk_020001D4;
u32 sub_0811086C(u8 *p)
{
    sub_0810FDC0(p);
    sub_0811CD08(p + 0xD8C);
    gUnk_020001D4 = 0;
    return 1;
}
