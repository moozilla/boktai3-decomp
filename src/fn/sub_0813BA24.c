#include "global.h"
/* Tentative role: initialize a hidden effect sprite. */
typedef struct Sprite { u32 flags; u8 pad4[11]; u8 mode,size; u8 pad11[0x17]; } Sprite;
typedef struct State { u8 pad0[0x744]; void *graphics; Sprite sprite; u8 active,ticks; } State;
void *sub_08215184(s32);
void sub_08217DE0(Sprite*,void*,s32);
void sub_08217EC4(Sprite*,s32,s32);
void sub_08217EEC(Sprite*,void*,s32);
void sub_0813BA24(State *s) {
 Sprite *sprite;
 s->graphics=sub_08215184(0x1c1c);
 sprite=&s->sprite;
 sub_08217DE0(sprite,s->graphics,0);
 sub_08217EC4(sprite,-4,-4);
 sub_08217EEC(sprite,s->graphics,4);
 sprite->flags|=1; sprite->mode=1; sprite->size=20;
 s->active=0; s->ticks=0;
}
