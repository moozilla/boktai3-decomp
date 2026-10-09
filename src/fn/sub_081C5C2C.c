#include "global.h"
void *sub_0821A520(u32,u32);
void sub_082196C4(void *,void *);
void sub_0821983C(u8 *,void *,u32,u32,u32,u32,u32,u32);
void sub_081C5AF0(u8 *);
void sub_081C5B6C(u8 *);
void sub_081C5C2C(u8 *s)
{
    struct Blob {u32 words[8];};
    u32 key=0xCB05;
    struct Blob *p=sub_0821A520(key,0xA2DB);
    struct Blob *first,*last;
    u32 zero,sixty,loopzero;
    u16 bits;
    u16 *dst;
    s32 i;
    u8 *entry;
    *(struct Blob *)(s+0x58)=*p;
    first=(struct Blob *)(s+0x58);
    sub_082196C4(first,p);
    p=sub_0821A520(key,0x1289);
    *(struct Blob *)(s+0x38)=*p;
    sub_082196C4(s+0x38,p);
    p=sub_0821A520(key,0x530D);
    *(struct Blob *)(s+0x18)=*p;
    last=(struct Blob *)(s+0x18);
    sub_082196C4(last,p);
    sub_0821983C(s+0x258,last,0x8A,0x10,zero=0,zero,sixty=0x3C,zero);
    dst=(u16 *)(s+0x27A);
    bits=0xFFE8;
    *dst=bits;
    sub_0821983C(s+0x1F8,first,0,0x10,3,zero,sixty,zero);
    loopzero=0;
    entry=s+0x78;
    i=3;
    do {
        sub_0821983C(entry,s+0x38,0,0x10,loopzero,loopzero,0x3C,loopzero);
        entry+=0x60;
    } while(--i>=0);
    sub_081C5AF0(s);
    sub_081C5B6C(s);
}
