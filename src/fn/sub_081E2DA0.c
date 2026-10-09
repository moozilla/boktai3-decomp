#include "global.h"
extern u16 gUnk_03004BD8;
extern u8 gUnk_03004FC0[];
void sub_082156B8(u32);
void sub_0821656C(u32,u32,u32,u32,u32);
void *sub_0821A520(u32,u32);
void sub_082161B4(u32,u32,void *,u32,u32,u32,u32 *);
void sub_082164AC(u32,void *,u32);
void sub_081E2DA0(void)
{
    u32 id=0xA597,pal=0x81B;
    u32 zero,one,value;
    void *p;
    u16 *src,*dest;
    s32 i,j;
    sub_082156B8(0);
    sub_082156B8(2);
    sub_0821656C(2,0,0,0,zero=0);
    sub_082156B8(3);
    sub_0821656C(3,0,0,0,zero);
    one=1;
    {u16 *g=&gUnk_03004BD8;u32 a=~1;u32 b=*g;a&=b;a&=~2;*g=a;}
    p=sub_0821A520(0xC091,id);
    value=zero;
    sub_082161B4(0,0,p,0,zero,one,&value);
    sub_082164AC(0,p,4);
    sub_0821656C(0,0,0,0,zero);
    src=(u16 *)((u8 *)sub_0821A520(0x92B3,pal)+0x1B4);
    for(i=13;i<=15;) {
        u32 offset=i*32;
        s32 next=i+1;
        dest=(u16 *)(gUnk_03004FC0+offset);
        for(j=15;j>=0;j--) { *dest=*src;src++;dest++; }
        i=next;
    }
}
