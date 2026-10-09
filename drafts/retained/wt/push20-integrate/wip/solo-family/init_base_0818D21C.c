#include "global.h"
struct Obj {
    u8 pad0[0x1c]; s16 x,y,z; u8 pad22[0xf8]; u8 angle; u8 pad11B[0x19];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142,dx,dy,dz,pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u8 *parent; u8 pad160[0x70];
    void (*callback)(struct Obj *); u16 pad1D4,timer; u8 phase,pad1D9; s8 multiplier; u8 done,pad1DC; u8 byte1DD;
};
extern u8 *gUnk_02000710;
extern const s16 gUnk_086149C4[];
u32 sub_082215E4(s32,s32);
s32 Div(s32,s32);
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_0818D384(struct Obj *);
void sub_0818D21C(struct Obj *p)
{
    s32 sign;
    p->angle=sub_082215E4(*(s16 *)(gUnk_02000710+0x30)-p->x,*(s16 *)(gUnk_02000710+0x34)-p->z);
    p->phase=160-p->angle;
    gUnk_03005308=(gUnk_03005308+1)&1023;
    sign=-1;
    if(gUnk_0203B400[gUnk_03005308]&1) sign=1;
    p->multiplier=sign;
    p->done=1;
    p->startX=p->x; p->startY=p->y; p->startZ=p->z;
    p->endX=p->x+Div(800*gUnk_086149C4[(p->angle+64)&255],4096);
    p->endY=*(u16 *)(gUnk_02000710+0x32)+220;
    p->endZ=p->z+Div(800*gUnk_086149C4[p->angle],4096);
    p->dx=p->endX-p->startX; p->dy=p->endY-p->startY; p->dz=p->endZ-p->startZ;
    p->callback=sub_0818D384;
}
