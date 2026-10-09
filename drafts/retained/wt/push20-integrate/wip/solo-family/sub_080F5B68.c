#include "global.h"
struct Obj {
    u8 pad0[6]; u8 rotation; u8 pad7[0x15]; s16 x,y,z; u8 pad22[0xf7]; u8 flags119,angle,pad11B[3],byte11E,byte11F,pad120[0x14];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142; s16 dx,dy,dz; u16 pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u16 *target; u8 *parent; u8 pad164[0x18];
    void (*callback)(struct Obj *); u32 pad180; s32 timer; u8 pad188[3],done,pad18C,byte1DD,pad18E[0x96]; void (*callback224)(struct Obj *);
};
struct Vec { s32 x:16,y:16,z:16; };
s32 Div(s32,s32);
void sub_080F60B8(struct Obj *);
void sub_080F5B68(struct Obj *p)
{
    struct Vec v;
    u16 *target=p->target;
    v.x=target[0]-p->a;
    v.y=target[1]-p->b;
    v.z=target[2]-p->c;
    p->rotation+=p->byte1DD;
    p->x=(((u16 *)&v)[0]+p->startX)+Div(p->dx*p->timer,100);
    p->y=(v.y+p->startY)+Div(p->dy*p->timer,100);
    p->z=(((u16 *)&v)[2]+p->startZ)+Div(p->dz*p->timer,100);
    p->timer++;
    if(p->timer>100) {
        p->done=1; p->rotation=128; p->byte11E=1; p->byte11F=1; p->flags119|=1;
        p->callback224=sub_080F60B8;
    }
}
