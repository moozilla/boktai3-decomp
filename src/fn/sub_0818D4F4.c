#include "global.h"
struct Obj {
    u8 pad0[6]; u8 rotation; u8 pad7[0x15]; s16 x,y,z; u8 pad22[0xf8]; u8 angle; u8 pad11B[0x19];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142; s16 dx,dy,dz; u16 pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u8 *parent; u8 pad160[0x70];
    void (*callback)(struct Obj *); u16 pad1D4; s16 timer; u8 pad1D8[3],done,pad1DC; u8 byte1DD;
};
s32 Div(s32,s32);
void sub_0818D590(struct Obj *);
void sub_0818D4F4(struct Obj *p)
{
    p->x=p->startX+Div(p->dx*p->timer,20);
    p->y=p->startY+Div(p->dy*p->timer,20);
    p->z=p->startZ+Div(p->dz*p->timer,20);
    p->timer--;
    if(p->timer==0) { p->done=1; p->callback=sub_0818D590; }
}
