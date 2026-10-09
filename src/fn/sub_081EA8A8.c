#include "global.h"
void *sub_0821A520(u32,u32);
void sub_082196C4(u8 *,void *);
void sub_0821983C(u8 *,u8 *,u32,u32,u32,u32,u32,u32);
void sub_081EA9B4(u8 *);
void sub_081EA8A8(u8 *s)
{
    void *p=sub_0821A520(0xCB05,0x731D);
    u8 *obj=s+0x4E0;
    u32 zero;
    u32 two;
    u32 one;
    sub_082196C4(obj,p);
    sub_0821983C(s+0x18,obj,1,0x10,zero=0,0,0,0);
    sub_0821983C(s+0xA0,obj,2,0x10,zero,zero,zero,zero);
    sub_0821983C(s+0x128,obj,11,0x10,two=2,zero,zero,zero);
    sub_0821983C(s+0x1B0,obj,11,0x10,two,zero,zero,zero);
    sub_0821983C(s+0x458,obj,0,0x10,zero,zero,zero,zero);
    sub_0821983C(s+0x238,obj,7,0x10,zero,zero,zero,zero);
    sub_0821983C(s+0x2C0,obj,3,0x10,zero,zero,zero,zero);
    sub_0821983C(s+0x348,obj,7,0x30,one=1,zero,zero,zero);
    sub_0821983C(s+0x3D0,obj,3,0x30,one,zero,zero,zero);
    sub_081EA9B4(s);
}
