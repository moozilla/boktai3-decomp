#include "global.h"
void sub_08163F14(u8 *,u32,u32,u32,u32);
void sub_08165ACC(u8 *);
void sub_08220D78(void *,void *,u32,u32,u32);
void sub_0821980C(void *,void *,u32,u32);
void sub_08163EA0(u8 *,void *);
void sub_0816AF5C(void);
void sub_0816AC64(u8 *s)
{
    u32 zero;
    u8 *a,*b,*initial;
    u16 *p;
    sub_08030BF8();
    sub_08033468();
    sub_08033590();
    sub_08030F20(1,1,28,6);
    sub_08030C84(*(u32 *)(s+0x58));
    sub_08030B78(1);
    sub_08163F14(s,*(u32 *)(s+0x18),0,2,*(u32 *)(s+0x2C));
    sub_08165ACC(s);
    *(u32 *)(s+0x3BE8)&=~1;
    p=(u16 *)(s+0x3C00);
    zero=0;
    *p=0xFFA8;
    *(u16 *)(s+0x3C02)=0x20;
    initial=s+0x3BE0;
    b=s+0xC4;
    sub_08220D78(initial,b,4,2,zero);
    a=s+0x3D00;
    sub_0821980C(a,b,0x1D,1);
    sub_08220D78(a,b,10,1,zero);
    sub_08030DBC(0);
    sub_08163EA0(s,sub_0816AF5C);
}
