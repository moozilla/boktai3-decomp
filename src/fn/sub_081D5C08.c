#include "global.h"
extern u16 gUnk_03004BD8;
extern u8 gUnk_03004FC0[],gUnk_03004CC0[];
void *sub_0821A520(u32,u32);
void sub_082161B4(u32,u32,void *,u32,u32,u32,u32 *);
void sub_08215A74(u32,u32);
void sub_08216520(u32,u32,u32);
void sub_08215EB4(u32,u32,u32,u32,u32,u32,u32,u32);
u32 sub_081D5C08(u8 *s,u32 value)
{
    u32 mask=0xF00;
    u16 *reg=&gUnk_03004BD8;
    void *p,*q;
    u32 color;
    u16 *dst;
    s32 i;
    u32 zero,one;
    struct Fields { u8 pad[0x18]; u16 first; u8 pad2[6]; u16 second,third; } *fields;
    mask|=*reg;
    *reg=mask;
    p=sub_0821A520(0xC091,0x3536);
    *(void **)(s+0x18)=p;
    sub_082161B4(1,0,p,0,0,1,&value);
    sub_08215A74(2,0);
    sub_08216520(2,0,0);
    q=(u8 *)sub_0821A520(0x92B3,0x204)+0x14;
    *(void **)(s+0x1C)=q;
    dst=(u16 *)gUnk_03004FC0;
    CpuSet(q,dst,0x30);
    color=0x73FF;
    i=15;
    dst+=0x4F;
    for(;i>=0;i--) {
        *dst=color;
        dst--;
    }
    zero=0;
    sub_082161B4(2,0,*(void **)(s+0x18),0,zero,one=1,&value);
    fields=(struct Fields *)gUnk_03004CC0;
    fields->first=0x10;
    fields->second=zero;
    fields->third=zero;
    sub_08215A74(3,3);
    sub_08215EB4(3,4,6,8,9,one,zero,4);
    return 0;
}
