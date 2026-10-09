#include "global.h"
struct Obj {
    u8 pad0[6]; u8 rotation; u8 pad7[0x15]; s16 x,y,z; u8 pad22[0xf8]; u8 angle; u8 pad11B[0x19];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142; s16 dx,dy,dz; u16 pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u8 *parent; u8 pad160[0x18];
    void (*callback)(struct Obj *); u8 padCallback[0x8]; s32 timer; u8 pad188[2]; s8 multiplier; u8 done,pad18C,byte1DD;
};
s32 Div(s32,s32);
void sub_080F558C(struct Obj *);
void sub_080F5504(struct Obj *p)
{
    p->x=p->startX+Div(p->dx*p->timer,20);
    p->y=p->startY+Div(p->dy*p->timer,20);
    p->z=p->startZ+Div(p->dz*p->timer,20);
    p->timer++;
    if(p->timer>20) p->callback=sub_080F558C;
}
