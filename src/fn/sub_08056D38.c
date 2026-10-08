#include "global.h"
extern u32 gUnk_030053F4;
void sub_0821AAD8(u32 *);
void sub_0821B4C8(u32 *, u32, u32);
void sub_08056D38(void)
{
    u32 buf[2];
    sub_0821AAD8(buf);
    if (gUnk_030053F4 & 0x800)
        sub_0821B4C8(buf, 0, 1);
    else
        sub_0821B4C8(buf, 0, 0);
}
