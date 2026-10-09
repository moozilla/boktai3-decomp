#include "global.h"
void sub_0822B2F8(u32);
void sub_0821FE40(u8 *);
void sub_0821A0C0(u8 *);
static inline void frame(u8 *p) { *(u16 *)(p+0x28)=*(u16 *)(p+0xb6)>>2; }
s32 sub_081DAF50(u8 *p)
{
    u16 *delay=(u16 *)(p+0xb4);
    if(*delay) (*delay)--;
    else {
        u16 *age;
        u32 value;
        frame(p);
        value=*(u16 *)(p+0xb6);
        age=(u16 *)(p+0xb6);
        if(value==0) { *(u32 *)(p+0x18)&=~1; if(*(s16 *)(p+0xba)==0) sub_0822B2F8(0x195); }
        if(*age<=7 && *(s16 *)(p+0xb8)==0) sub_0821FE40(p+0x60);
        (*age)++;
        if(*age>23) sub_0821A0C0(p);
    }
    return 0;
}
