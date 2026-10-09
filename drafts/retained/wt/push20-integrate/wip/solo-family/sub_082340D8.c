#include "global.h"
struct Pair { u32 a,b; };
void sub_082151E4(u8 *,u32);
void sub_082144A4(u8 *,u8 *,u32);
s32 sub_0821D6F4(struct Pair *,u32);
void sub_08215284(u8 *,u32);
void sub_082340D8(u8 *p)
{
    u8 *resource=p+0x48,*sprite;
    sub_082151E4(resource,0xe74b);
    sprite=p+0x1c;
    sub_082144A4(sprite,resource,1);
    *(u16 *)(sprite+0x10)=12;
    *(struct Pair *)(p+0x38)=*(struct Pair *)(p+0x64);
    if(sub_0821D6F4((struct Pair *)(p+0x38),128)) { sub_08215284(resource,402); p[0x23]=1; }
    else p[0x23]=2;
}
