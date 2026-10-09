#include "global.h"
struct Data { u32 words[8]; };
struct Data *sub_0821A520(u32,u32);
void sub_082196C4(struct Data *,struct Data *);
void sub_0821983C(u8 *,struct Data *,u32,u32,u32,u32,u32,void *);
void sub_082359CC(u8 *p)
{
    struct Data data;
    struct Data *source=sub_0821A520(0xcb05,0x5d04);
    u32 frame,zero,x;
    u16 *coords,*dest;
    data=*source;
    sub_082196C4(&data,source);
    frame=*(u16 *)(p+0x34c)==0?47:48;
    coords=(u16 *)(p+0x84);
    x=coords[0]-8;
    dest=(u16 *)(p+0x7c);
    zero=0;
    dest[0]=x;
    x=*(u16 *)(p+0x86)-40;
    dest[1]=x;
    x=*(u16 *)(p+0x88);
    dest[2]=x;
    sub_0821983C(p+0x1c,&data,frame+104,50,1,zero,60,coords);
    p[0x51]=34; p[0x50]=34;
}
