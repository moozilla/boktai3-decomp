#include "global.h"
typedef struct Screen Screen;
struct Screen {u8 task[40];u16 dirty;u8 pad2a[0x284];u16 reveal,revealStep,top,bottom;u8 pad2b6[8];u16 revealDuration,closeDelay,start,end,blend,fade,fadeStep;void(*state)(Screen*);void(*effect)(Screen*);u32 pad2d4;u16 timer;u8 pad2da[2];u32 args[10],script;};
extern u16 gUnk_03005260[],gUnk_03004BD8;
void sub_082279A8(int,int,int,int,int,int,int);void sub_0821AD08(u32,void*);void sub_0821A0C0(void*);void sub_0822B2F8(int);
void sub_0800EC0C(Screen*);void sub_0800EE5C(Screen*);void sub_0800EF90(Screen*);void sub_0800E838(Screen*);
#define StatusScreen_CheckConfirm sub_0800E7D8
int StatusScreen_CheckConfirm(Screen *p){
 if((gUnk_03005260[1]&8)||(gUnk_03005260[1]&1)){
 sub_082279A8(3,4,4,4,4,65535,0);{register void (*state)(Screen*) asm("r1")=sub_0800EC0C;p->state=state;}p->timer=0;return 1;
 }return 0;
}
