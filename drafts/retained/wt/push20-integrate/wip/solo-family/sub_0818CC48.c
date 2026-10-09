#include "global.h"
struct Obj {
    u8 pad0[6]; u8 rotation; u8 pad7[0x15]; s16 x,y,z; u8 pad22[0xf8]; u8 angle; u8 pad11B[0x19];
    u16 endX,endY,endZ,pad13A,startX,startY,startZ,pad142; s16 dx,dy,dz; u16 pad14A;
    u16 fx,fy,fz,pad152,a,b,c,pad15A; u8 *parent; u8 pad160[0x70];
    void (*callback)(struct Obj *); u16 pad1D4; s16 timer; u8 pad1D8[2]; s8 multiplier; u8 pad1DB[2]; s8 byte1DD;
};
extern u8 *gUnk_02000710;
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_0818CD80(struct Obj *);
void sub_0818CE48(struct Obj *);
static inline s32 absolute(s32 x) { if(x<0) x=-x; return x; }
static inline s8 chooseSign(u32 x) { if(x&1) return -1; return 1; }
void sub_0818CC48(struct Obj *p)
{
    if((p->parent[0x5c2]&3)==0) p->byte1DD--;
    p->rotation+=p->byte1DD;
    if(absolute(p->byte1DD)>9) {
        u8 *player;
        s32 sign;
        u32 value,one;
        p->startX=p->x; p->startY=p->y; p->startZ=p->z;
        player=gUnk_02000710;
        p->endX=*(u16 *)(player+0x30);
        p->endY=*(u16 *)(player+0x32)+220;
        p->endZ=*(u16 *)(player+0x34);
        p->dx=p->endX-p->startX; p->dy=p->endY-p->startY; p->dz=p->endZ-p->startZ;
        p->timer=0;
        gUnk_03005308=(gUnk_03005308+1)&1023;
        value=gUnk_0203B400[gUnk_03005308];
        p->multiplier=chooseSign(value);
        if(absolute(p->dx)<absolute(p->dz)) p->callback=sub_0818CD80;
        else p->callback=sub_0818CE48;
    }
}
