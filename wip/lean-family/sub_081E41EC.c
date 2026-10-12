#include "global.h"
/* Tentative role: arm the idle animation transition at its resting position. */
struct S {u8 pad[0xddc];u32 flags;u8 p1[4];s8 scaleX;u8 p2[0xdf8-0xde5];s16 x,y,z;u8 p3[0xf6f-0xdfe];u8 enabled;u8 p4[0xf84-0xf70];u16 delay;u8 p5[10];void (*fn)(void*);void (*scaleFn)(void*);u32 unknown;s32 timer;};
extern u8 gUnk_03005430[];
void sub_081E42C4(void*);void sub_081E41B0(void*);
static inline s32 absolute(s32 x){return x<0?-x:x;}
void sub_081E41EC(struct S *p){
 if(p->x==25 && p->z==0 && absolute(p->y-102)<=9){
 p->timer=0;p->fn=sub_081E42C4;
 if(gUnk_03005430[14]==5 || gUnk_03005430[14]==0 || gUnk_03005430[14]==4)p->delay=1800;else p->delay=10800;
 if(p->enabled && p->scaleX!=127 && (p->flags&16))p->scaleFn=sub_081E41B0;
 }else p->fn=0;
}
