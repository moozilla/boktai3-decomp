#include "global.h"
void sub_082151E4(u8 *,u32);
void sub_08215284(u8 *,u32);
void sub_082144A4(u8 *,u8 *,u32);
void *sub_0821A520(u32,u32);
void sub_081625C0(u8 *s,u8 *source,void *a,void *b)
{
    u8 *p=s+0x18;
    u8 *entry;
    s32 i;
    u32 zero;
    struct Pair { u32 x,y; };
    struct Flags { u32 field; u8 pad[12]; };
    u32 key;
    sub_082151E4(p,0x7E5F);
    p+=0x1C;
    key=0x6250;
    sub_082151E4(p,key);
    sub_08215284(p,0x116);
    p+=0x1C;
    sub_082151E4(p,key);
    sub_08215284(p,0x118);
    p+=0x1C;
    sub_082151E4(p,key);
    sub_08215284(p,0x119);
    p+=0x1C;
    sub_082151E4(p,key);
    sub_08215284(p,0x11A);
    p+=0x1C;
    sub_082151E4(p,key);
    sub_08215284(p,0x11B);
    entry=s+0xC4;
    i=0;
    do {
        sub_082144A4(entry,p,1);
        *(u16 *)(entry+0x10)=(zero=0);
        *(struct Pair *)(entry+0x1C)=*(struct Pair *)(source+0x20);
        entry[7]=1;
        ((struct Flags *)(s+0x484))[i].field=zero;
        entry+=0x2C;
        i++;
    } while(i<=15);
    *(void **)(s+0x5A4)=sub_0821A520(0x922E,0x981D);
    *(u8 **)(s+0x594)=source;
    *(void **)(s+0x58C)=a;
    *(void **)(s+0x590)=b;
    *(u32 *)(s+0x584)=0;
}
