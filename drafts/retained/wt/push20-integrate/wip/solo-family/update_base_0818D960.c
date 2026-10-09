#include "global.h"
struct Obj {
    u8 pad0[6]; u8 rotation; u8 pad7[0x15]; s16 x,y,z; u8 pad22[0xf8]; u8 angle; u8 pad11B[0x19];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142; s16 dx,dy,dz; u16 pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u8 *parent; u8 pad160[0x70];
    void (*callback)(struct Obj *); u16 pad1D4; s16 timer; u8 pad1D8[5]; u8 byte1DD;
};
s32 Div(s32,s32);
void sub_0818DB10(struct Obj *);
void sub_0818D960(struct Obj *p)
{
    u8 *owner=p->parent;
    p->rotation+=p->byte1DD;
    p->x=p->startX+Div(p->dx*p->timer,100);
    p->y=p->startY+Div(p->dy*p->timer,100);
    p->z=p->startZ+Div(p->dz*p->timer,100);
    p->timer++;
    if(p->timer>100) {
        p->callback=sub_0818DB10; p->timer=0;
        p->a=*(u16 *)(owner+0x5ac)-p->fx-p->startX;
        p->b=*(u16 *)(owner+0x5ae)-p->fy-p->startY;
        p->c=*(u16 *)(owner+0x5b0)-p->fz-p->startZ;
        p->endX=p->a+p->startX;
        p->endY=p->b+p->startY;
        p->endZ=p->c+p->startZ;
        p->startX=p->x;
        p->startY=p->y;
        p->startZ=p->z;
        p->dx=p->endX-p->startX;
        p->dy=p->endY-p->startY;
        p->dz=p->endZ-p->startZ;
        p->a=*(u16 *)(owner+0x5ac);
        p->b=*(u16 *)(owner+0x5ae);
        p->c=*(u16 *)(owner+0x5b0);
    }
}
