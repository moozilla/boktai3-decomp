#include "global.h"
void sub_082164AC(u32,u32,u32);
void sub_0821656C(u32,u32,u32,u32,u32);
void sub_081BC664(u8 *);
void sub_08220D78(u8 *,u8 *,u32,u32,u32);
void sub_0821980C(u8 *,u8 *,u32,u32);
void sub_081BC3B4(u8 *);
void sub_081BC5F0(u8 *);
void sub_081BC468(u8 *);
void sub_08215284(u8 *,u32);
void sub_08030DBC(u32);
void sub_081BCCD4(void);
void sub_081BC694(u8 *,void (*)(void));
void sub_081BC820(u8 *s)
{
    u32 zero,latezero,mask,mask2,mask3,value;
    struct Word8 {u8 pad[8];u32 flags;};
    u8 *resource;
    u32 *p,*q,*initial;
    s32 i,j;
    sub_082164AC(0,*(u32 *)(s+0x8E0),4);
    sub_0821656C(0,0,0,0,zero=0);
    sub_081BC664(s);
    value=*(u32 *)(s+0x60);
    value&=(mask=~1);
    *(u32 *)(s+0x60)=value;
    sub_08220D78(s+0x58,(resource=s+0x18),5,1,zero);
    *(u32 *)(s+0x240)&=mask;
    sub_0821980C(s+0x238,resource,0x12,1);
    *(u16 *)(s+0x8EE)=zero;
    resource+=0x48;
    initial=(u32 *)(s+0x2A0);
    i=3;
    do {
        *initial&=mask;
        initial=(u32 *)((u8 *)initial+0x60);
    } while(--i>=0);
    sub_081BC3B4(s);
    sub_081BC5F0(s);
    *(u32 *)(s+0x5A0)&=(mask2=~1);
    q=(u32 *)(resource+0x5A0);
    p=(u32 *)(resource+0x3C0);
    j=3;
    do {
        *p&=mask2;
        *q&=mask2;
        q=(u32 *)((u8 *)q+0x60);
        p=(u32 *)((u8 *)p+0x60);
    } while(--j>=0);
    latezero=0;
    sub_081BC468(s);
    *(u16 *)(s+0x908)=latezero;
    *(u16 *)(s+0x90A)=latezero;
    ((struct Word8 *)(s+0x778))->flags&=(mask3=~1);
    *(u32 *)(s+0x898)&=mask3;
    sub_08215284(s+0x8C4,0x1B4);
    ((struct Word8 *)(s+0x7D8))->flags&=mask3;
    ((struct Word8 *)(s+0x838))->flags&=mask3;
    sub_08030DBC(4);
    *(u16 *)(s+0x8FA)=latezero;
    *(u16 *)(s+0x8FC)=latezero;
    *(u16 *)(s+0x8FE)=latezero;
    *(u16 *)(s+0x900)=latezero;
    *(u16 *)(s+0x8F8)=latezero;
    sub_081BC694(s,sub_081BCCD4);
}
