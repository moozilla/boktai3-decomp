#include "global.h"
struct Obj {
    u8 pad0[6]; u8 rotation; u8 pad7[0x15]; s16 x,y,z; u8 pad22[0xf8]; u8 angle; u8 pad11B[0x19];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142; s16 dx,dy,dz; u16 pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u8 *parent; u8 pad160[0x14];
    void (*callback)(struct Obj *); u8 padCallback[0xc]; s32 timer; u8 pad188[2]; s8 multiplier; u8 done,pad18C,byte1DD;
};
extern const s16 gUnk_086149C4[];
s32 Div(s32,s32);
void sub_080F4FA8(struct Obj *);
void sub_080F4E30(struct Obj *p)
{
    s32 wave,position;
    const s16 *table;
    p->rotation+=p->byte1DD;
    table=gUnk_086149C4;
    position=p->timer;
    wave=(table[*(u8 *)&p->timer]>>3)*p->multiplier;
    p->x=p->startX+Div(p->dx*position,128)+wave;
    p->y=p->startY+Div(p->dy*p->timer,128);
    p->z=p->startZ+Div(p->dz*p->timer,128);
    p->timer++;
    if(p->timer>128) p->callback=sub_080F4FA8;
}
