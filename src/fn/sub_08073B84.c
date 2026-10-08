#include "global.h"
extern u8 *gUnk_020001A0;
void sub_08072BA8(void);
u32 sub_08073B84(u8 *p, u16 v)
{
    gUnk_020001A0 = p;
    sub_08072BA8();
    *(u16 *)(p + 0x24) = v;
}
