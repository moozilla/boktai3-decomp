#include "global.h"
/* Tentative role: update four effects moving toward a common center. */
typedef struct Effect {
 u32 flags; u8 pad4[0x14]; u16 x; u8 pad1a[2]; u16 y; u8 pad1e[0xa];
 u8 active, ticks, speed, kind; s16 dx; u8 pad2e[2]; s16 dy; u8 pad32[2];
} Effect;
typedef struct State {
 u8 pad0[0x9a8]; void *graphics; u8 active; u8 pad9ad[3]; Effect effects[4];
 u16 x; u8 pada82[2]; u16 y;
} State;
extern const s16 gUnk_086149C4[];
s32 sub_082215E4(s32,s32);
void sub_08217EEC(Effect *,void *,s32);
static inline s32 magnitude(s32 v) { if(v<0) return -v; return v; }
static inline s32 scale(s32 v) { if(v>=0) return v>>12; return -((-v)>>12); }
void sub_082391A8(State *s) {
 s32 count, i;
 if(!s->active) return;
 count=0;
 for(i=0;i<4;i++) {
  Effect *e=&s->effects[i];
  if(e->active) {
   Effect *draw=e;
   if(magnitude(draw->dx)<=15 && magnitude(draw->dy)<=15) { draw->flags|=1; e->active=0; }
   else {
    s32 angle=sub_082215E4(e->dx,e->dy);
    e->dx-=scale(e->speed*gUnk_086149C4[(angle+64)&255]);
    e->dy-=scale(e->speed*gUnk_086149C4[angle&255]);
    draw->x=s->x+(u16)e->dx; draw->y=s->y+(u16)e->dy;
    if(++e->ticks==4) sub_08217EEC(draw,s->graphics,e->kind+1);
    count++;
   }
  }
 }
 if(!count) s->active=0;
}
