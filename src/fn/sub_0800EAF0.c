#include "global.h"
// MATCHING_CONCESSION: register-pinning
// The optimization override reproduces this body; it is not historical compiler evidence.
typedef struct Screen Screen;
struct Screen {u8 task[40];u16 dirty;u8 pad2a[0x284];u16 reveal,revealStep,top,bottom;u8 pad2b6[8];u16 revealDuration,closeDelay,start,end,blend,fade,fadeStep;void(*state)(Screen*);void(*effect)(Screen*);u32 pad2d4;u16 timer;u8 pad2da[2];u32 args[10],script;};
extern u16 gUnk_03005260[],gUnk_03004BD8;
void sub_082279A8(int,int,int,int,int,int,int);void sub_0821AD08(u32,void*);void sub_0821A0C0(void*);void sub_0822B2F8(int);
void sub_0800EC0C(Screen*);void sub_0800EE5C(Screen*);void sub_0800EF90(Screen*);void sub_0800E838(Screen*);
int sub_0800E7D8(Screen*);void sub_0800ED30(Screen*);void sub_0800EBB0(Screen*);
#define StatusScreen_Reveal sub_0800EAF0
void StatusScreen_Reveal(Screen *p){
 if(!sub_0800E7D8(p)){
 if(!p->timer){sub_0822B2F8(225);p->top=0;p->bottom=160;p->reveal=256;p->revealStep=-2;p->pad2d4=(u32)sub_0800EF90;p->blend=0x1084;p->fade=0;p->fadeStep=4;p->effect=sub_0800ED30;}
 if(p->timer++>=*(u16*)((u8*)p+0x2B8)){{register void(*state)(Screen*) asm("r0")=sub_0800EBB0;asm("" : "+r"(p) : "r"(state));p->state=state;}p->timer=0;}
 p->dirty|=15;
 }
}

// CFLAGS: -O2 -mthumb-interwork -fno-expensive-optimizations
