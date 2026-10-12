#include "global.h"
// MATCHING_CONCESSION: register-pinning
typedef struct Screen Screen;
struct Screen {u8 task[40];u16 dirty;u8 pad2a[0x284];u16 reveal,revealStep,top,bottom;u8 pad2b6[8];u16 revealDuration,closeDelay,start,end,blend,fade,fadeStep;void(*state)(Screen*);void(*effect)(Screen*);u32 pad2d4;u16 timer;u8 pad2da[2];u32 args[10],script;};
extern u16 gUnk_03005260[],gUnk_03004BD8;
void sub_082279A8(int,int,int,int,int,int,int);void sub_0821AD08(u32,void*);void sub_0821A0C0(void*);void sub_0822B2F8(int);
void sub_0800EC0C(Screen*);void sub_0800EE5C(Screen*);void sub_0800EF90(Screen*);void sub_0800E838(Screen*);
#define StatusScreen_Close sub_0800E838
void StatusScreen_Close(Screen *p){
 if(!p->timer){p->start=12;p->end=16;p->blend=0x1084;p->fade=0;p->fadeStep=2;p->effect=sub_0800EE5C;}
 if(p->timer==p->closeDelay){sub_0821AD08(p->script,p->args);sub_0821A0C0(p);{register u16 *flags asm("r2")=&gUnk_03004BD8;register s32 mask asm("r0");asm("" : "+r"(flags));mask=~256;asm("" : "+r"(mask));mask&=*flags;asm("" : "+r"(mask));*flags=mask;}}
 {register u32 flags asm("r1")=p->dirty;register u32 result asm("r0");asm("" : "+r"(flags));result=4;result|=flags;{register u32 mask asm("r1")=65532;asm("" : : "r"(mask),"r"(result));p->dirty=result&mask;}}p->timer++;
}
