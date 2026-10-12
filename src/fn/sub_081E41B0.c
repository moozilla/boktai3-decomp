#include "global.h"
/* Tentative role: arm the idle animation transition at its resting position. */
struct S {u8 pad[0xddc];u32 flags;u8 p1[4];s8 scaleX,scaleY;u8 p2[0xdf8-0xde6];s16 x,y,z;u8 p3[0xf6f-0xdfe];u8 enabled;u8 p4[0xf84-0xf70];u16 delay;u8 p5[10];void (*fn)(void*);void (*scaleFn)(void*);u32 unknown;s32 timer;};
extern u8 gUnk_03005430[];
void sub_081E42C4(void*);
static inline s32 absolute(s32 x){return x<0?-x:x;}
void sub_081E41B0(struct S*p){s32 x=p->scaleX; s32 y=p->scaleY;x+=2;y+=2;if(x>127){y=127;x=127;p->scaleFn=0;}p->scaleX=x;p->scaleY=y;}
