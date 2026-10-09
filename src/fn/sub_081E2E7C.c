#include "global.h"
void *sub_0821A520(u32,u32);
void sub_082196C4(void *,void *);
void sub_0821983C(u8 *,void *,u32,u32,u32,u32,u32,void *);
void sub_081E2E7C(u8 *s)
{
    struct Blob {u32 words[8];};
    struct Coords {u16 a,b,c,d;} coords;
    struct Blob *p=sub_0821A520(0xCB05,0xDF11);
    struct Blob *obj;
    u32 zero,sixty;
    *(struct Blob *)(s+0x18)=*p;
    obj=(struct Blob *)(s+0x18);
    sub_082196C4(obj,p);
    coords.a=0;
    coords.b=0;
    coords.c=0;
    sub_0821983C(s+0x38,obj,15,0x30,zero=0,0,sixty=0x3C,&coords);
    sub_0821983C(s+0x98,obj,11,0x30,zero,zero,sixty,&coords);
    sub_0821983C(s+0xF8,obj,2,0x30,zero,zero,sixty,&coords);
    sub_0821983C(s+0x1B8,obj,5,0x30,zero,zero,sixty,&coords);
    sub_0821983C(s+0x158,obj,0,0x30,zero,zero,sixty,&coords);
}
