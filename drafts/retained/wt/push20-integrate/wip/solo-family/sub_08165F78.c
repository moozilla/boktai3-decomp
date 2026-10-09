#include "global.h"
struct Vec { s16 x,y,z,pad; };
struct Elem { u8 pad0[0x20]; struct Vec pos; u8 pad28[0x38]; };
struct State { struct Vec from,to; union { u32 enabled; struct { s16 x,y; } axes; } bend; u32 pad14; u16 timer,duration; u8 active,pad1D[3]; };
extern const s16 gUnk_086149C4[];
s32 Div(s32,s32);
static inline s32 scale(s32 v) { if(v>=0) return v>>12; return -((-v)>>12); }
u32 sub_08165F78(u8 *p,u32 i)
{
    struct Elem *e=(struct Elem *)(p+0x2aa0)+i;
    struct State *s=(struct State *)(p+0x43ec)+i;
    s32 remaining;
    if(s->active==0) return 1;
    s->timer++;
    if(s->timer>=s->duration) { e->pos=s->to; s->active=0; return 1; }
    remaining=s->duration-s->timer;
    e->pos.x=Div(s->from.x*remaining+s->to.x*s->timer,s->duration);
    e->pos.y=Div(s->from.y*remaining+s->to.y*s->timer,s->duration);
    if(s->bend.enabled) {
        s32 wave=gUnk_086149C4[Div(s->timer*128,s->duration)&255];
        e->pos.x+=scale(s->bend.axes.x*wave);
        e->pos.y+=scale(s->bend.axes.y*wave);
    }
    return 0;
}
