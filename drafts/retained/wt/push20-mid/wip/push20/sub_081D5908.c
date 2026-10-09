#include "global.h"
struct A {u32 a[8];};
struct V {s16 x,y,z,w;};
struct A *sub_0821A520(u32,u32);
void sub_082196C4(struct A *,struct A *);
void sub_0821983C(u8 *,struct A *,u32,u32,u32,u32,u32,struct V *);
u32 sub_081D5908(u8 *s)
{
    struct V v;
    struct A *a=sub_0821A520(0xCB05,0xE2AB);
    struct A *b=(struct A *)(s+0x4C);
    u8 *dst;
    u8 *other;
    s32 i,j;
    s32 stride;
    u32 zero;
    *b=*a;
    sub_082196C4(b,a);
    v.x=0;v.y=0;v.z=0;v.w=0;
    i=0;
    stride=0;
    other=s+0x2AC;
    do {
        dst=s+(stride+0x54C);
        j=1;
        do {
            sub_0821983C(dst,b,0,0x10,0,2,0x3C,&v);
            dst+=0x60;
            --j;
        } while(j>=0);
        sub_0821983C(other,b,0x14,0x10,0,2,0x3C,&v);
        other+=0x60;
        stride+=0xC0;
        ++i;
    } while(i<=6);
    v.x=0;v.y=0;v.z=0;v.w=0;
    dst=s+0xA8C;
    other=s+0xBAC;
    j=1;
    do {
        sub_0821983C(dst,b,0,0x10,0,2,0x3C,&v);
        sub_0821983C(other,b,0,0x10,0,2,0x3C,&v);
        other+=0x60;dst+=0x60;--j;
    } while(j>=0);
    sub_0821983C(s+0xB4C,b,0x15,0x10,0,2,0x3C,&v);
    zero=0;
    v.x=zero;v.y=zero;v.z=zero;v.w=zero;
    dst=s+0xC6C;
    sub_0821983C(dst,b,0x3E,0x10,0,2,0x3C,&v);
    *(u32 *)(dst+8)|=1;
    v.x=zero;v.y=zero;v.z=zero;v.w=zero;
    sub_0821983C(s+0xCCC,b,0x40,0x10,0,2,0x3C,&v);
    *(u32 *)(s+0xCD4)|=1;
    v.x=zero;v.y=zero;v.z=zero;v.w=zero;
    sub_0821983C(s+0x24C,b,0x3F,0x10,0,2,0x3C,&v);
    v.x=zero;v.y=zero;v.z=zero;v.w=zero;
    sub_0821983C(s+0x12C,b,0x48,0x10,0,2,0x3C,&v);
    v.x=zero;v.y=zero;v.z=zero;v.w=zero;
    sub_0821983C(s+0x1EC,b,1,0x10,0,2,0x3C,&v);
    v.x=zero;v.y=zero;v.z=zero;v.w=zero;
    sub_0821983C(s+0x18C,b,0x41,0x10,0,2,0x3C,&v);
    v.x=zero;v.y=zero;v.z=zero;v.w=zero;
    sub_0821983C(s+0x6C,b,0x46,0x10,0,2,0x3C,&v);
    *(u16 *)(s+0x8E)=s[0x48]<<4;
    v.x=zero;v.y=zero;v.z=zero;v.w=zero;
    sub_0821983C(s+0xCC,b,0x47,0x10,0,2,0x3C,&v);
    dst=s+0xD2C;
    CpuSet(*(void **)(s+0xB4),dst,0x04000008);
    *(void **)(s+0xB4)=dst;
    *(void **)(s+0x114)=dst;
}
