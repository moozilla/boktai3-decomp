#include "global.h"
void *sub_0821A520(u32,u32);
void sub_082196C4(void *,void *);
void sub_0821983C(u8 *,void *,u32,u32,u32,u32,u32,u32);
void sub_081C6448(u8 *);
void sub_081C62B4(u8 *);
void sub_081C6498(u8 *s)
{
    struct Blob {u32 words[8];};
    u32 bank=0xCB05,zero,sixty;
    struct Blob *p=sub_0821A520(bank,0x1289),*first;
    u8 *obj;
    s32 i;
    *(struct Blob *)(s+0x18)=*p;
    first=(struct Blob *)(s+0x18);
    sub_082196C4(first,p);
    p=sub_0821A520(bank,0xE2AB);
    *(struct Blob *)(s+0x38)=*p;
    sub_082196C4(s+0x38,p);
    sub_0821983C(s+0x58,first,0,0x10,1,zero=0,sixty=0x3C,zero);
    sub_0821983C(s+0xB8,first,0,0x10,2,zero,sixty,zero);
    {
        u32 loopzero=0;
        obj=s+(sixty+0xDC);
        for(i=5;i>=0;i--) {
            sub_0821983C(obj,s+0x38,0,0x31,2,loopzero,0x3C,loopzero);
            obj+=0x60;
        }
    }
    sub_081C6448(s);
    sub_081C62B4(s);
    *(u32 *)(s+0x60)|=1;
}
