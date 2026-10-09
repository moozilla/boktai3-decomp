#include "global.h"
struct Elem { u32 flags; u8 pad4[0x28]; };
struct Obj { u8 pad0[0xa8]; struct Elem elems[4]; u8 pad158[0x330]; u16 endX,endY,endZ,pad48E; s16 dx,dy,dz,pad496; u16 startX,startY,startZ; u8 pad49E[0x16]; u16 timer; u8 pad4B6[0x6ce]; u16 x,y,z; u8 padB8A[0x14e]; void (*callback)(struct Obj *); };
s32 Div(s32,s32);
void sub_080F2AAC(struct Obj *);
void sub_080F29C4(struct Obj *p)
{
    u32 age;
    p->x=p->startX+Div(p->dx*p->timer,16);
    p->y=p->startY+Div(p->dy*p->timer,16);
    p->z=p->startZ+Div(p->dz*p->timer,16);
    age=p->timer++;
    if((u16)age>16) {
        struct Elem *e;
        void (*callback)(struct Obj *);
        u32 one;
        p->timer=0;
        p->x=p->endX; p->y=p->endY; p->z=p->endZ;
        callback=sub_080F2AAC;
        one=1;
        e=p->elems;
        do { e->flags|=one; e++; } while((s32)e<=(s32)(p->elems+3));
        p->callback=callback;
    }
}
