#include "global.h"
/* Tentative role: place an effect at a fixed radius opposite the supplied angle. */
typedef struct Point { u16 x,z,y,pad6; } Point;
typedef struct State { u8 pad0[0x7fc]; u32 flags; u8 pad800[2]; u8 angle; u8 pad803[0x15]; Point pos; u8 pad820[0x40]; u8 active; } State;
extern const s16 gUnk_086149C4[];
static inline s32 scale(s32 v) { if(v>=0) return v>>12; return -((-v)>>12); }
void sub_0813C198(State *s,s32 angle,Point *pos) {
 s->flags&=~1;
 angle=(angle+128)&255;
 s->pos=*pos;
 s->pos.x+=scale(170*gUnk_086149C4[(angle+64)&255]);
 s->pos.z+=230;
 s->pos.y+=scale(170*gUnk_086149C4[angle&255]);
 s->angle=-96-angle;
 s->active=1;
}
