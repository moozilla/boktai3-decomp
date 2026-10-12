#include "global.h"
/* Tentative role: initialize the orbiting effect and its update callback. */
typedef struct Point { u32 a,b; } Point;
typedef struct Sprite { u32 flags; u8 pad4[3]; u8 mode,xscale,yscale; u8 pada[6]; u16 frame; u8 pad12[0xa]; Point pos; u8 pad24[8]; } Sprite;
typedef struct State { u8 pad0[0x30]; Point pos; u8 pad38[0x3e0]; u8 mode; u8 pad419[0x3e3]; Sprite sprite; u8 animation[0x38]; u8 active; u8 pad861[3]; void (*callback)(void*); } State;
void sub_082151E4(void*,s32);
void sub_082144A4(Sprite*,void*,s32);
void sub_08215284(void*,s32);
void sub_0813BFE0(void*);
void sub_0813C26C(State *s) {
 Sprite *sprite=&s->sprite; void *animation=s->animation;
 sub_082151E4(animation,0x2110); sub_082144A4(sprite,animation,67);
 sprite->frame=4;
 if(s->mode==6) sub_08215284(animation,81); else sub_08215284(animation,75);
 sprite->mode=1;sprite->yscale=127;sprite->xscale=127;
 sprite->pos=s->pos; s->callback=sub_0813BFE0; s->active=0;
}
