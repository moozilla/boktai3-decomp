#include "global.h"
struct Elem { u32 flags; u8 pad4[11]; u8 attr,attr2; u8 pad11[7]; u16 x,y,z; u8 pad1E[10]; u16 angle,radius; };
struct Owner { u8 pad[0x6c]; struct Elem elems[8]; u32 handle; };
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
u32 sub_08215184(u32);
void sub_08217DE0(struct Elem *,u32,s32);
void sub_08217EC4(struct Elem *,s32,s32);
void sub_08217EEC(struct Elem *,u32,s32);
void sub_08217ECC(struct Elem *,s32);
static inline s32 scale(s32 x) { if(x>=0) x>>=12; else x=-((-x)>>12); return x; }
static inline u16 rnd(u16 *table) { gUnk_03005308=(gUnk_03005308+1)&1023; return table[gUnk_03005308]; }
void sub_08234138(struct Owner *p)
{
    u32 seed;
    s32 i;
    gUnk_03005308=(gUnk_03005308+1)&1023;
    seed=*(u8 *)((gUnk_03005308<<1)+(u32)gUnk_0203B400);
    p->handle=sub_08215184(0x1c1e);
    for(i=0;i<8;i++) {
        struct Elem *e=&p->elems[i];
        u16 *table;
        sub_08217DE0(e,p->handle,0);
        sub_08217EC4(e,-4,-4);
        sub_08217EEC(e,p->handle,7);
        sub_08217ECC(e,1);
        table=gUnk_0203B400;
        p->elems[i].angle=(seed+i*32+(rnd(table)&15))&255;
        p->elems[i].radius=(rnd(table)&511)+256;
        e->x=*(u16 *)((u8 *)p+0x64)+scale(p->elems[i].radius*gUnk_086149C4[(p->elems[i].angle+64)&255]);
        e->y=*(u16 *)((u8 *)p+0x66)+300;
        e->z=*(u16 *)((u8 *)p+0x68)+scale(p->elems[i].radius*gUnk_086149C4[(u8)p->elems[i].angle]);
        e->attr=2; e->attr2=236;
    }
}
