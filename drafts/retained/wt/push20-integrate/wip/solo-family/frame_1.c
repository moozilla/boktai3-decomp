#include "global.h"
struct Effect { u32 flags; u8 pad4[0x14]; u16 x,y,z; u8 pad1E[10]; u16 age,active,dx,dy,dz,pad32; void *data; void (*update)(struct Effect *); };
void sub_08217EEC(struct Effect *,void *,u32);
static inline u32 frame(u16 age) { return ((age>>2)&1)+2; }
void sub_08234810(struct Effect *p)
{
    u32 age=p->age+1;
    p->age=age;
    if(p->age>15) { p->flags|=1; p->active=0; }
    else {
        u32 dx,x,dy,y;
        sub_08217EEC(p,p->data,frame(age));
        dx=p->dx; x=p->x; p->x=dx+x;
        dy=p->dy; y=p->y; p->y=y+dy;
        if(!(p->age&7)) p->dy=dy+1;
    }
}
