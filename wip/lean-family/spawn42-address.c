#include "global.h"
/* Tentative role: periodically spawn an inward-moving effect in a four-slot ring. */
typedef struct Point { u16 x, pad2, y, pad6; } Point;
typedef struct Effect { u32 flags; u8 pad4[0x14]; Point pos; u8 pad20[8]; u8 active,ticks,speed,kind; s16 dx; u8 pad2e[2]; s16 dy; u8 pad32[2]; } Effect;
typedef struct State { u8 pad0[0x9a8]; void *graphics; u8 active, next, ticks, pad9af; Effect effects[4]; } State;
extern const s16 gUnk_086149C4[];
extern u8 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern u8 *gUnk_02000710;
void sub_08217EEC(Effect*,void*,s32);
static inline s32 random_value(void) {register u8 *table asm("r2"); register u16 *address asm("r0"); gUnk_03005308=(gUnk_03005308+1)&1023; table=gUnk_0203B400; address=(u16 *)(table+(gUnk_03005308<<1)); return *address;}
static inline s32 random_first(void) { u8 *table=gUnk_0203B400; gUnk_03005308=(gUnk_03005308+1)&1023; return *(u16 *)(table+(gUnk_03005308<<1)); }
static inline s32 scale(s32 v) { if(v>=0) return v>>12; return -((-v)>>12); }
void sub_0813C42C(State *s,Point *pos) {
 s32 step=*(s16 *)(gUnk_02000710+0x876);
 s->ticks=step+2+s->ticks;
 if(s->ticks>39) {
  Effect *e=&s->effects[s->next]; Effect *draw=e;
  register s32 angle asm("r12"); s32 radius;
  draw->flags&=~1;
  e->kind=2; sub_08217EEC(draw,s->graphics,2);
  angle=random_first()&255; radius=(random_first()&63)+128;
  e->dx=scale(radius*gUnk_086149C4[(angle+64)&255]);
  e->dy=scale(radius*gUnk_086149C4[angle]);
  draw->pos=*pos;
  draw->pos.x+=(u16)e->dx;
  draw->pos.y+=(u16)e->dy;
  e->speed=(random_value()&3)+14;
  e->ticks=0; e->active=1;
  if(++s->next>3) s->next=0;
  s->ticks-=40; s->active=1;
 }
}
