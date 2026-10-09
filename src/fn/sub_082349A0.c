#include "global.h"
struct Elem { u32 flags; u8 pad4[0x14]; u16 x,y,z; u8 pad1E[10]; u16 age,active,dx,dy,dz; u16 pad32; void *data; void (*update)(struct Elem *); };
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_08217EEC(struct Elem *,void *,u32);
void sub_08234960(struct Elem *);
s32 Mod(s32,s32);
static inline s32 rnd(u16 *table) { s32 v; gUnk_03005308=(gUnk_03005308+1)&1023; v=table[gUnk_03005308]; return v>>3; }
void sub_082349A0(struct Elem *p)
{
    u16 *table;
    u32 zero;
    p->flags&=~1;
    sub_08217EEC(p,p->data,6);
    table=gUnk_0203B400;
    p->x=Mod(rnd(table),16)+8;
    zero=0;
    p->y=Mod(rnd(table),16)+24;
    p->z=zero; p->dx=zero;
    p->dy=-(Mod(rnd(table),2)+2);
    p->dz=zero; p->active=1; p->age=zero;
    p->update=sub_08234960;
}
