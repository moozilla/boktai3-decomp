#include "global.h"
void sub_082156B8(u32);
void *sub_0821A520(u32,u32);
void sub_082161B4(u32,u32,void *,u32,u32,u32,u32 *);
void sub_0821656C(u32,u32,u32,u32,u32);
void sub_08215A74(u32,u32);
void sub_081F0E24(u8 *s)
{
    s32 kind=*(s16 *)(s+0x5EE);
    struct Scratch {u32 fill,value;} scratch;
    u32 zero,key;
    void *p,*q,*r;
    u8 *dst;
    u32 *fill;
    if(kind) {
        sub_082156B8(1);
        *(void **)(s+0x3A4)=(p=sub_0821A520(0xC091,0x42FF));
        scratch.value=0;
        sub_082161B4(3,0,p,0,0,1,&scratch.value);
        sub_0821656C(1,0,0,0,0x60);
    } else {
        sub_082156B8(3);
        *(void **)(s+0x3A4)=(p=sub_0821A520(0xC091,0x42FF));
        scratch.value=kind;
        sub_082161B4(2,0,p,0,kind,1,&scratch.value);
        sub_08215A74(3,2);
        sub_0821656C(3,0,0,0,0x60);
    }
    key=0x92B3;
    q=(u8 *)sub_0821A520(key,0xB499)+0x94;
    CpuSet(q,s+0x408,0x40);
    r=(u8 *)sub_0821A520(key,0xDDFB)+0x94;
    CpuSet(r,s+0x488,0x40);
    *(u32 *)(s+0x268)=0xFFF9C000;
    *(u32 *)(s+0x26C)=(zero=0);
    *(u32 *)(s+0x270)=0x1C000;
    dst=s+0x278;
    scratch.fill=zero;
    fill=&scratch.fill;
    CpuSet(fill,dst,0x05000002);
    *(u32 *)(s+0x284)=zero;
    *(u32 *)(s+0x280)=0x5000;
    *(u32 *)(s+0x288)=zero;
}
