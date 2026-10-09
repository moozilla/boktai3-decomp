#include "global.h"
extern u32 gUnk_030053F4;
void sub_082260A4(s32);
void sub_0821FE40(u8 *);
void sub_0821A0C0(u8 *);
#define H(o) (*(u16 *)(p+(o)))
void sub_082352C8(u8 *p)
{
    sub_082260A4((16-H(0x34e))*2);
    if(H(0x34e)<=5) sub_0821FE40(p+0x94);
    else if(H(0x34e)==6) gUnk_030053F4&=~1;
    H(0x34e)++;
    if(H(0x34e)>15) sub_0821A0C0(p);
}
