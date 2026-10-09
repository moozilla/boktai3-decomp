#include "global.h"
struct Data { u32 words[8]; };
struct Vec { s32 x:16,y:16,z:16; };
struct Data *sub_0821A520(u32,u32);
void sub_082196C4(struct Data *,struct Data *);
void sub_0821983C(u8 *,struct Data *,u32,u32,u32,u32,u32,struct Vec *);
void sub_081BD934(struct Vec *,s32);
void CpuSet(const void *,void *,u32);
void sub_0805E000(u8 *,u32);
void sub_0805E3FC(u8 *);
void sub_0805E270(u8 *);
static inline void setup(u8 *p,struct Data *data,u32 frame,u32 layer,struct Vec *pos) { sub_0821983C(p,data,frame,layer,0,0,60,pos); }
struct Obj { u8 pad0[0x1518]; struct Data dataA,dataB; };
void sub_081BF67C(struct Obj *p)
{
    struct Data *resource;
    struct Vec v;
    s16 *pos;
    s32 i;
    u32 zero;
    u8 *first,*second;
    resource=sub_0821A520(0xcb05,0x5d04);
    p->dataB=*resource;
    sub_082196C4(&p->dataB,resource);
    resource=sub_0821A520(0xcb05,0x530d);
    p->dataA=*resource;
    sub_082196C4(&p->dataA,resource);
    v.x=0; v.y=0; pos=(s16 *)&v; v.z=0;
    setup((u8 *)p+0x18,&p->dataA,0xd2,0x30,&v);
    setup((u8 *)p+0x78,&p->dataA,0x41,0x30,&v);
    pos[0]+=8;
    setup((u8 *)p+0xd8,&p->dataA,0x42,0x30,&v);
    zero=0;
    CpuSet(&zero,&v,0x05000002);
    setup((u8 *)p+0x138,&p->dataA,0x40,0x30,&v);
    setup((u8 *)p+0x198,&p->dataA,0xd3,0x30,&v);
    setup((u8 *)p+0x1f8,&p->dataA,0x63,0x30,&v);
    first=(u8 *)p+0x318; second=(u8 *)p+0x918;
    for(i=0;i<16;second+=0x60,first+=0x60,i++) {
        sub_081BD934(&v,i);
        setup(first,&p->dataB,0x151,0x30,&v);
        sub_081BD934(&v,i+16);
        setup(second,&p->dataB,0x151,0x30,&v);
    }
    pos[0]=0; pos[1]=128;
    setup((u8 *)p+0x1098,&p->dataB,0xcf,0x30,&v);
    pos[0]=216; pos[1]=104;
    setup((u8 *)p+0xf18,&p->dataB,0x190,0x30,&v);
    sub_0805E000((u8 *)p+0xf18,0x373);
    pos[0]=216; pos[1]=80;
    setup((u8 *)p+0xfd8,&p->dataB,0x191,0x30,&v);
    sub_0805E000((u8 *)p+0xfd8,0x373);
    pos[0]=216; pos[1]=56;
    setup((u8 *)p+0x1038,&p->dataB,0x192,0x30,&v);
    sub_0805E000((u8 *)p+0x1038,0x373);
    first=(u8 *)p+0x10f8;
    for(i=0;i<8;first+=0x60,i++) {
        sub_081BD934(&v,i);
        setup(first,&p->dataA,0x50,0x31,&v);
    }
    setup((u8 *)p+0x14b8,&p->dataA,0x4e,0x31,&v);
    pos[0]=144; pos[1]=32;
    setup((u8 *)p+0x1458,&p->dataA,0x4d,0x30,&v);
    setup((u8 *)p+0x13f8,&p->dataA,0x4d,0x31,&v);
    pos[0]=0; pos[1]=0;
    setup((u8 *)p+0x258,&p->dataA,0x4b,0x11,&v);
    sub_0805E3FC((u8 *)p+0x1558);
    sub_0805E270((u8 *)p+0x164c);
}
