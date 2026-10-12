#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec{s16 x,y,z,pad;};struct Sprite{u8 pad[8];u32 flags;u8 padc[20];struct Vec position;u8 pad28[12];u8 scaleX,scaleY;u16 angle;};struct Effect{u32 pad0;u8 state,entered,pad6,mode;u32 pad8,timer;u16 duration;u8 index,pad13[5];struct Vec position;u8 pad20[4];struct Sprite sprite;};
void sub_080168B8(struct Effect*);
#define FloatingEffect_Bob sub_08016944
void FloatingEffect_Bob(void*m,struct Effect*p){if(p->entered){p->entered=0;p->sprite.flags&=~1;}if(p->timer<=2)p->position.y+=16;else p->position.y-=16;sub_080168B8(p);if(p->timer>3){register int state asm("r0")=1;asm("" : : "r"(state));p->state=state;p->entered=state;p->timer=0;}else p->timer++;}
