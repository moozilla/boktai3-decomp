#include "global.h"
struct Obj {
    u8 pad0[6]; u8 rotation; u8 pad7[0x15]; s16 x,y,z; u8 pad22[0xf8]; u8 angle; u8 pad11B[0x19];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142; s16 dx,dy,dz; u16 pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u16 *target; u8 *parent; u8 pad164[0x18];
    void (*callback)(struct Obj *); u32 pad180; s32 timer; u8 pad188[5]; s8 byte1DD;
};
static inline s32 absolute(s32 x) { if(x<0) x=-x; return x; }
void sub_080F59E0(struct Obj *);
void sub_080F5964(struct Obj *p)
{
    if((*(u32 *)(p->parent+0xd80)&3)==0) {
        p->byte1DD--;
        if(absolute(p->byte1DD)>15) p->byte1DD=-15;
    }
    p->rotation+=p->byte1DD;
    p->timer--;
    if(p->timer<0) {
        p->timer=0;
        if(absolute(p->byte1DD)==15) p->callback=sub_080F59E0;
    }
}
