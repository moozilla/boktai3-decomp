#include "global.h"
struct Pair { u32 a,b; };
void sub_082151E4(u8 *,u32);
void sub_082144A4(u8 *,u8 *,u32);
void sub_08215284(u8 *,u32);
static inline void setFields(u8 *p,u32 bit) { p[7]=bit; *(u32 *)p|=bit; }
void sub_081DAFDC(u8 *sprite,u8 *resource,struct Pair *pos,s32 frame)
{
    sub_082151E4(resource,0x8639);
    sub_082144A4(sprite,resource,0);
    *(u16 *)(sprite+0x10)=0;
    frame+=0x116;
    sub_08215284(resource,frame);
    *(struct Pair *)(sprite+0x1c)=*pos;
    setFields(sprite,1);
}
