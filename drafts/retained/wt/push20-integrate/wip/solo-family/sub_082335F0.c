#include "global.h"
struct Pair { u32 a,b; };
struct Elem { u32 flags; u8 pad4[20]; struct Pair pos; u8 pad20[8]; u8 state,age; u8 pad2A[2]; };
struct Owner { u8 pad[0x8c]; struct Pair pos; u8 pad94[16]; struct Elem elems[8]; u8 index,timer; };
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
static inline u8 rnd(void) { gUnk_03005308=(gUnk_03005308+1)&1023; return *(u8 *)((gUnk_03005308<<1)+(u32)gUnk_0203B400); }
void sub_082335F0(struct Owner *p)
{
    u32 next=p->timer+1;
    u32 mask=255;
    p->timer=next;
    if((next&mask)>1) {
        p->elems[p->index].flags&=~1;
        p->elems[p->index].pos=p->pos;
        *(u16 *)&p->elems[p->index].pos+=rnd()-127;
        ((u16 *)&p->elems[p->index].pos)[1]+=rnd()-127;
        ((u16 *)&p->elems[p->index].pos)[2]+=rnd()-127;
        p->elems[p->index].state=1;
        p->elems[p->index].age=0;
        p->index++;
        if((p->index&mask)>7) p->index=0;
        p->timer=0;
    }
}
