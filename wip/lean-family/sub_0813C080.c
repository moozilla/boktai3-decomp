#include "global.h"
/* Tentative role: place and launch a directional effect. */
typedef struct Point { u16 x,z,y,pad6; } Point;
typedef struct Sprite { u8 pad0[0x1c]; Point pos; } Sprite;
typedef struct State { u8 pad0[0x3a4]; u8 facing; u8 pad3a5[0x457]; Sprite sprite; u8 pad820[0x20]; u32 pad840; u8 animation[0x10]; void *resource; s16 dx; u8 pad85a[2]; s16 dy; } State;
extern const s16 gUnk_086149C4[];
void sub_0813C000(u32,u32*,u32*);
void sub_08220C8C(void*,void*,s32,u8,u8);
void sub_0813C040(State*);
static inline s32 scale(s32 v) { if(v>=0) return v>>12; return -((-v)>>12); }
void sub_0813C080(State *s,u32 mode,Point *pos) {
 Sprite *sprite=&s->sprite; s32 angle; u32 a,b;
 angle=((s->facing+5)&7)<<5;
 sprite->pos=*pos; sprite->pos.z+=190;
 sprite->pos.x+=scale(240*gUnk_086149C4[(angle+64)&255]);
 sprite->pos.y+=scale(240*gUnk_086149C4[angle&255]);
 s->dx=scale(10*gUnk_086149C4[(angle+64)&255]);
 s->dy=scale(10*gUnk_086149C4[angle&255]);
 sub_0813C000(mode,&a,&b);
 sub_08220C8C(s->animation,s->resource,0,a,b);
 sub_0813C040(s);
}
