#include "global.h"
/* Tentative role: start/stop and orbit a companion effect around the actor. */
typedef struct Point { u16 x,z,y,pad6; } Point;
typedef struct Sprite { u32 flags; u8 pad4[0x14]; Point pos; u8 pad20[8]; } Sprite;
typedef struct State { u8 pad0[0x1c]; u32 mode; u8 pad20[0x10]; Point pos; u8 pad38[0x4fa]; u16 enabled; u8 pad534[0x298]; void *graphics; Sprite sprite; u8 active,ticks; } State;
extern const s16 gUnk_086149C4[];
void sub_08217EEC(Sprite*,void*,s32);
static inline s32 scale(s32 v) { if(v>=0) return v>>12; return -((-v)>>12); }
void sub_08238814(State *s) {
 if(s->active) {
  s32 angle;
  if(s->mode!=1 || !s->enabled) { s->sprite.flags|=1; s->active=0; return; }
  angle=(s->ticks*5)&255;
  s->sprite.pos.x=s->pos.x+scale(56*gUnk_086149C4[(angle+64)&255]);
  s->sprite.pos.z=scale(70*gUnk_086149C4[angle])+350+s->pos.z;
  s->sprite.pos.y=s->pos.y-scale(56*gUnk_086149C4[(angle+64)&255]);
  s->ticks++;
 } else if(s->mode==1 && s->enabled) {
  s->sprite.pos=s->pos; s->sprite.pos.x+=56; s->sprite.pos.z+=350;
  s->sprite.flags&=~1; sub_08217EEC(&s->sprite,s->graphics,16);
  s->ticks=0;s->active=1;
 }
}
