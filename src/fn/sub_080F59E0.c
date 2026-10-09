#include "global.h"
struct Obj {
    u8 pad0[6]; u8 rotation; u8 pad7[0x15]; s16 x,y,z; u8 pad22[0xf8]; u8 angle; u8 pad11B[0x19];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142; s16 dx,dy,dz; u16 pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u16 *target; u8 *parent; u8 pad164[0x18];
    void (*callback)(struct Obj *); u32 pad180; s32 timer; u8 pad188[5]; u8 byte1DD;
};
s32 Div(s32,s32);
void sub_080F5B68(struct Obj *);
void sub_080F59E0(struct Obj *p)
{
    p->rotation+=p->byte1DD;
    p->x=p->startX+Div(p->dx*p->timer,100);
    p->y=p->startY+Div(p->dy*p->timer,100);
    p->z=p->startZ+Div(p->dz*p->timer,100);
    p->timer++;
    if(p->timer>100) {
        p->callback=sub_080F5B68; p->timer=0;
        p->a=p->target[0]-p->fx-p->startX;
        p->b=p->target[1]-p->fy-p->startY;
        p->c=p->target[2]-p->fz-p->startZ;
        p->endX=p->startX+p->a;
        p->endY=p->startY+p->b;
        p->endZ=p->startZ+p->c;
        p->startX=p->x;
        p->startY=p->y;
        p->startZ=p->z;
        p->dx=p->endX-p->startX;
        p->dy=p->endY-p->startY;
        p->dz=p->endZ-p->startZ;
        p->a=p->target[0];
        p->b=p->target[1];
        p->c=p->target[2];
    }
}
