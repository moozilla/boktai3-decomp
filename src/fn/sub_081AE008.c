#include "global.h"
void *sub_0821A520(u32,u32);
void sub_082196C4(u8 *,void *);
void sub_0821983C(u8 *,u8 *,u32,u32,u32,u32,u32,u8 *);
void sub_08220D78(u8 *,u32,u32,u32,u32);
void sub_081ADEAC(u8 *,u8 *,u8 *);
void sub_081ADDF0(u8 *,u8 *,u8 *);
void sub_081ADBF8(void);
void sub_080216A4(u8 *,u8 *,void (*)(void),u32);
void sub_081AE008(u8 *s,u8 *t)
{
    u8 *coords=t+0x80;
    u32 width=0x280, height=0x300;
    u32 zero=0;
    u8 *obj;
    void *p;
    *(u16 *)coords=width;
    *(u16 *)(coords+2)=height;
    *(u16 *)(coords+4)=width;
    p=sub_0821A520(0xCB05,0xB3DE);
    obj=t+0x60;
    sub_082196C4(obj,p);
    sub_0821983C(t,obj,0,0,2,2,0x3C,coords);
    sub_08220D78(t,obj,11,1,zero);
    *(u16 *)(t+0x3A)=0x6F;
    *(u8 **)(t+0x48)=s+0x718;
    sub_081ADEAC(s,t+0x90,coords);
    t+=0xA8;
    sub_081ADDF0(s,t,coords);
    sub_080216A4(s,s+0x6C4,sub_081ADBF8,*(u16 *)(s+0x590));
}
