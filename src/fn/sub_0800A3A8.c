#include "global.h"
// MATCHING_CONCESSION: register-pinning
// The empty constraints keep loop temporaries separate across the pause-state transition.
struct Slot {u16 id;u8 state,type,kind,active,remove,entered,done;u8 pad9;u16 timer;u8 rest[148];};
struct Manager {u8 task[24];u8 paused;u8 pad19[19];struct Slot slots[16];};
struct Scene {u8 pad[14];u8 mode;};
struct Actor {u8 pad[0x448];u32 flags;u8 pad44C[0xe8];u16 mode,pad536,active;};
extern struct Scene gUnk_03005430;extern u32 gUnk_030053F4;extern struct Actor *gUnk_02000580;extern s32 gUnk_02000478;extern u16 gUnk_02000534,gUnk_02000500;
extern void *gUnk_08604A00[2][3];void sub_08009F34(struct Manager *,struct Slot *);void sub_08249240(struct Manager *,struct Slot *,void *);
static inline s32 stateMode(void){struct Actor *a=gUnk_02000580;if(a){u32 active=a->active;s32 yes=0;if(active&&a->mode==4)yes=1;return yes;}return 0;}
static inline s32 globalMode(void){u32 active=gUnk_02000534;s32 yes=0;if(active&&gUnk_02000500==4)yes=1;return yes;}
static inline s32 actorMode(void){struct Actor *a=gUnk_02000580;if(a){u32 mask=16;return a->flags&mask;}return 0;}
static inline void resetIdle(struct Manager *manager){register u32 one asm("r2")=1;u8 *p=&manager->slots[0].state;register s32 i asm("r6")=15;do{if(p[0]==0){p[0]=0;*(u16 *)(p+8)=0;p[5]=one;}p+=160;}while(--i>=0);}
#define CollisionController_UpdateAll sub_0800A3A8
s32 CollisionController_UpdateAll(struct Manager *input){
 struct Manager *manager=input;
 register s32 paused asm("r3")=0;s32 i;struct Slot *p;u32 mode=gUnk_03005430.mode;s32 available;
 if((u8)(mode-4)<=1||mode==0)goto pause;
 if(gUnk_030053F4&2048)available=stateMode();
 else{struct Actor *actor;if(gUnk_02000478>0)goto pause;actor=gUnk_02000580;if(actor){u32 *address=&actor->flags;register u32 mask asm("r1")=16;asm("" : : "r"(address),"r"(mask));if(*address&mask)goto pause;}available=globalMode();}
 asm("" : : "r"(available));
 if(available){pause:paused=1;if(manager->paused==0)resetIdle(manager);}
 else if(manager->paused==1)resetIdle(manager);
 manager->paused=paused;asm("" : "+r"(manager) : : "r4","r5","r6");
 {register void **table asm("r9")=&gUnk_08604A00[0][0];register u32 row asm("r8")=paused*12;u8 *control=&manager->slots[0].state;
 p=manager->slots;
 for(i=15;i>=0;control+=160,p++,i--)if(control[3]){
  if(control[4])sub_08009F34(manager,p);
  else{control[6]=0;sub_08249240(manager,p,*(void **)(control[0]*4+row+(u32)table));}
 }
 }
 return 0;
}
