#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a,b; };
void sub_0821FEB4(u8 *,u32,u32,u32,u32,struct Z *,struct Z *);
void sub_0821FF7C(u8 *,u32,u32,u32);
void sub_0821FF24(u8 *,u8 *,u32);
void sub_0821FF84(u8 *,void (*)(void),u8 *);
void sub_0821FE40(u8 *);
void sub_08104F98(void);
void sub_08105194(u8 *s,u32 a,u32 b,u32 c)
{
    struct Z v,w;
    u8 *q=s+0x58;
    *(u32 *)&w.a=0x640000;
    w.b.lo=0x40;
    v.a.lo=0x40;
    v.a.hi=0x3C;
    v.b.lo=0x40;
    *(u16 *)(q+4)=*(u32 *)(s+0x140);
    sub_0821FEB4(q,0,0x5001,0,0x10,&v,&w);
    sub_0821FF7C(q,a,b,c);
    sub_0821FF24(q,s+0x1C,0);
    sub_0821FF84(q,sub_08104F98,s);
    sub_0821FE40(q);
}
