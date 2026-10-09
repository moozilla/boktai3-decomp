#include "global.h"
void sub_0822B2F8(u32);
void sub_0821FE40(u8 *);
void sub_0821A0C0(u8 *);
s32 sub_082331E4(u8 *p)
{
    u16 *delay=(u16 *)(p+0xb4);
    if(*delay) (*delay)--;
    else {
        u16 *age=(u16 *)(p+0xb6);
        *(u16 *)(p+0x28)=*age>>2;
        if(*age==0) { *(u32 *)(p+0x18)&=~1; sub_0822B2F8(0x195); }
        if(*age<=7) sub_0821FE40(p+0x60);
        (*age)++;
        if(*age>23) sub_0821A0C0(p);
    }
    return 0;
}
