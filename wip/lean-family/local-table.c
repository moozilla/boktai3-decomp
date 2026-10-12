#include "global.h"
/* Tentative role: periodically spawn an inward-moving effect in a four-slot ring. */
typedef struct Point { u16 x, pad2, y, pad6; } Point;
typedef struct Effect { u32 flags; u8 pad4[0x14]; Point pos; u8 pad20[8]; u8 active,ticks,speed,kind; s16 dx; u8 pad2e[2]; s16 dy; u8 pad32[2]; } Effect;
typedef struct State { u8 pad0[0x9a8]; void *graphics; u8 active, next, ticks, pad9af; Effect effects[4]; } State;
extern const s16 gUnk_086149C4[];
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void sub_08217EEC(Effect*,void*,s32);
static inline s32 random_value(u16 *table) { gUnk_03005308=(gUnk_03005308+1)&1023; return table[gUnk_03005308]; }
static inline s32 scale(s32 v) { if(v>=0) return v>>12; return -((-v)>>12); }
void sub_0813C5B4(State *s,Point *pos) {
 if((++s->ticks & 3)==0) {
  Effect *e=&s->effects[s->next]; Effect *draw=e;
  s32 angle, radius; u16 *table;
  draw->flags&=~1;
  e->kind=4; sub_08217EEC(draw,s->graphics,4);
  table=gUnk_0203B400; angle=random_value(table)&255; radius=(random_value(table)&127)+256;
  e->dx=scale(radius*gUnk_086149C4[(angle+64)&255]);
  e->dy=scale(radius*gUnk_086149C4[angle]);
  draw->pos=*pos;
  draw->pos.x+=(u16)e->dx;
  draw->pos.y+=(u16)e->dy;
  e->speed=(random_value(gUnk_0203B400)&7)+28;
  e->ticks=0; e->active=1; s->active=1;
  if(++s->next>3) s->next=0;
 }
}
