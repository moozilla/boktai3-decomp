#include "global.h"
// MATCHING_CONCESSION: register-pinning
typedef struct Screen Screen;
struct Screen {u8 task[40];u16 dirty;u8 pad2a[0x284];u16 reveal,revealStep,top,bottom;u8 pad2b6[8];u16 revealDuration,closeDelay,start,end,blend,fade,fadeStep;void(*state)(Screen*);void(*effect)(Screen*);u32 pad2d4;u16 timer;u8 pad2da[2];u32 args[10],script;};
extern u16 gUnk_03005260[],gUnk_03004BD8;
void sub_082279A8(int,int,int,int,int,int,int);void sub_0821AD08(u32,void*);void sub_0821A0C0(void*);void sub_0822B2F8(int);
void sub_0800EC0C(Screen*);void sub_0800EE5C(Screen*);void sub_0800EF90(Screen*);void sub_0800E838(Screen*);
void sub_0800E77C(Screen*);int sub_0821656C(int,int,int,int,int);
#define StatusScreen_Update sub_0800F154
int StatusScreen_Update(Screen *p){
 register int zero asm("r4");
 if(p->state)p->state(p);if(p->effect)p->effect(p);if(p->pad2d4)((void(*)(Screen*))p->pad2d4)(p);
 {u16 next=1-*(u16*)((u8*)p+0x2AC);zero=0;asm("" : "+r"(zero));*(u16*)((u8*)p+0x2AC)=next;}sub_0800E77C(p);return sub_0821656C(0,0,0,0,zero);
}
