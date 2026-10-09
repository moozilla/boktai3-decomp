#include "global.h"
struct Elem { u32 flags; u8 pad4[0x14]; u16 x,y,z; u8 pad1E[10]; u16 age,active,dx,dy,dz; u16 pad32; void *data; void (*update)(struct Elem *); };
struct Owner { u8 pad[0x8c]; struct Elem elems[8]; };
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void sub_08234810(struct Elem *);
static inline s32 scale(s32 x) { if(x>=0) x>>=12; else x=-((-x)>>12); return x; }
static inline u16 mask8(s32 x) { return x&255; }
static inline s32 rnd(u16 *table) { s32 v; gUnk_03005308=(gUnk_03005308+1)&1023; v=table[gUnk_03005308]; return v>>3; }
void sub_08234868(struct Owner *p)
{
    u16 *table=gUnk_0203B400;
    u16 *e;
    s32 i;
    for(i=0,e=(u16 *)((u8 *)p+0xa4);i<8;e+=30,i++) {
        s32 angle, speed;
        e[0]=16;
        e[1]=32;
        angle=i*32;
        angle+=rnd(table)&15;
        angle-=8;
        speed=(rnd(table)&3)+4;
        e[10]=scale(speed*gUnk_086149C4[mask8(angle+64)]);
        e[11]=scale(speed*gUnk_086149C4[mask8(angle)]);
        p->elems[i].flags&=~1;
        e[9]=1;
        e[8]=0;
        p->elems[i].update=sub_08234810;
    }
}
