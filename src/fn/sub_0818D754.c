#include "global.h"
struct Obj {
    u8 pad0[0x1c]; s16 x,y,z; u8 pad22[0xf8]; u8 angle; u8 pad11B[0x19];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142,dx,dy,dz,pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u8 *parent; u8 pad160[0x70];
    void (*callback)(struct Obj *); u16 pad1D4,timer; u8 pad1D8[5]; u8 byte1DD;
};
extern u8 *gUnk_02000710;
extern const s16 gUnk_086149C4[];
u32 sub_082215E4(s32,s32);
s32 Div(s32,s32);
void sub_0818D8E0(struct Obj *);
void sub_0818D754(struct Obj *p)
{
    u8 *owner=p->parent;
    p->angle=sub_082215E4(*(s16 *)(gUnk_02000710+0x30)-p->x,*(s16 *)(gUnk_02000710+0x34)-p->z);
    p->byte1DD=0; p->callback=sub_0818D8E0;
    p->a=0; p->b=0; p->c=0;
    p->fx=*(u16 *)(owner+0x5ac)-p->x;
    p->fy=*(u16 *)(owner+0x5ae)-p->y;
    p->fz=*(u16 *)(owner+0x5b0)-p->z;
    p->startX=p->x; p->startY=p->y; p->startZ=p->z;
    p->endX=p->x+Div(800*gUnk_086149C4[(p->angle+64)&255],4096);
    p->endY=*(u16 *)(gUnk_02000710+0x32)+220;
    p->endZ=p->z+Div(800*gUnk_086149C4[p->angle],4096);
    p->dx=p->endX-p->startX; p->dy=p->endY-p->startY; p->dz=p->endZ-p->startZ;
    p->timer=0;
}
