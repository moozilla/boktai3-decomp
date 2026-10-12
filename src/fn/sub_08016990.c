#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec{s16 x,y,z,pad;};struct Sprite{u8 pad[8];u32 flags;u8 padc[20];struct Vec position;u8 pad28[12];u8 scaleX,scaleY;u16 angle;};struct Effect{u32 pad0;u8 state,entered,pad6,mode;u32 pad8,timer;u16 duration;u8 index,pad13[5];struct Vec position;u8 pad20[4];struct Sprite sprite;};
void sub_080168B8(struct Effect*);
extern void*gUnk_02000580[];
#define FloatingEffect_Wait sub_08016990
void FloatingEffect_Wait(void*m,struct Effect*p){if(p->entered){p->entered=0;p->sprite.flags&=~1;}sub_080168B8(p);if(p->timer>=p->duration){u32 mode=p->mode;if(mode==0){register u32 index asm("r0")=p->index;asm("" : : "r"(index));if(gUnk_02000580[index])p->state=6;else p->state=5;p->entered=1;p->timer=mode;}else if(mode==1){register int state asm("r0")=2;asm("" : : "r"(state));p->state=state;p->entered=mode;p->timer=0;}else if(mode==3){register int state asm("r0")=6;asm("" : : "r"(state));p->state=state;p->entered=1;p->timer=0;}}else p->timer++;}
