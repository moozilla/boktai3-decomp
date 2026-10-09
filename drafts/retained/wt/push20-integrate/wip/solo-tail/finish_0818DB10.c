#include "global.h"
struct Obj {
    u8 pad0[6]; u8 rotation; u8 pad7[0x15]; s16 x,y,z; u8 pad22[0xf7]; u8 flags119,angle,pad11B[3],byte11E,byte11F,pad120[0x14];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142; s16 dx,dy,dz; u16 pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u8 *parent; u8 pad160[0x70];
    void (*callback)(struct Obj *); u16 pad1D4; s16 timer; u8 pad1D8[3],done,pad1DC; u8 byte1DD;
};
struct Vec { u32 x:16,y:16,z:16; };
s32 Div(s32,s32);
static inline void finish(struct Obj *p,u32 one) { p->done=one; p->rotation=128; p->byte11E=one; p->byte11F=one; p->flags119|=one; }
void sub_0818DB10(struct Obj *p)
{
    struct Vec v;
    u8 *owner=p->parent;
    v.x=*(u16 *)(owner+0x5ac)-p->a;
    v.y=*(u16 *)(owner+0x5ae)-p->b;
    v.z=*(u16 *)(owner+0x5b0)-p->c;
    p->rotation+=p->byte1DD;
    p->x=(((u16 *)&v)[0]+p->startX)+Div(p->dx*p->timer,100);
    p->y=((s16)v.y+p->startY)+Div(p->dy*p->timer,100);
    p->z=(((u16 *)&v)[2]+p->startZ)+Div(p->dz*p->timer,100);
    p->timer++;
    if(p->timer>100) {
        finish(p,1);
    }
}
