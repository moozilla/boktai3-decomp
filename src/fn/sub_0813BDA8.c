#include "global.h"
/* Tentative role: initialize a pair of effect sprites. */
typedef struct Sprite { u32 flags; u8 pad4[11]; u8 mode,size; u8 pad11[0x17]; } Sprite;
typedef struct State { u8 pad0[0x744]; void *graphics; u8 pad748[0x2c]; void *resource; Sprite sprites[2]; u8 active,ticks; } State;
void *sub_08215184(s32);
void sub_08217DE0(Sprite*,void*,s32);
void sub_08217EC4(Sprite*,s32,s32);
void sub_08217EEC(Sprite*,void*,s32);
void sub_0813BDA8(State *s) {
 Sprite *sprite; s32 i;
 s->resource=sub_08215184(0x1c1c);
 sprite=s->sprites;
 for(i=1;i>=0;sprite++,i--) {
  sub_08217DE0(sprite,s->resource,17);
  sub_08217EC4(sprite,4,4);
  sub_08217EEC(sprite,s->graphics,74);
  sprite->mode=1; sprite->size=20;
 }
 s->active=0;s->ticks=0;
}
