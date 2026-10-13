#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Slot {u16 id;u8 state,type,kind,active,remove,entered,done;u8 pad9;u16 timer;u8 rest[148];};
struct Manager {u8 task[24];u8 paused;u8 pad19[3];void *graphics,*animationGraphics,*hitResource,*idleResource;struct Slot slots[16];};
struct Scene {u8 pad[14];u8 mode;};
struct Actor {u8 pad[0x448];u32 flags;u8 pad44C[0xe8];u16 mode,pad536,active;};
extern struct Scene gUnk_03005430;extern u32 gUnk_030053F4;extern struct Actor *gUnk_02000580;extern s32 gUnk_02000478;extern u16 gUnk_02000534,gUnk_02000500;
extern void *gUnk_08604A00[2][3];void sub_08009F34(struct Manager *,struct Slot *);void sub_08249240(struct Manager *,struct Slot *,void *);
static inline s32 stateMode(void){struct Actor *a=gUnk_02000580;if(a){u32 active=a->active;s32 yes=0;if(active&&a->mode==4)yes=1;return yes;}return 0;}
static inline s32 globalMode(void){u32 active=gUnk_02000534;s32 yes=0;if(active&&gUnk_02000500==4)yes=1;return yes;}
static inline s32 actorMode(void){struct Actor *a=gUnk_02000580;if(a){u32 mask=16;return a->flags&mask;}return 0;}
extern struct Manager *gUnk_0200003C;void *sub_08215184(u32);void *sub_0821A520(u32,u32);
#define CollisionController_InitManager sub_0800A534
s32 CollisionController_InitManager(struct Manager *manager){
 u32 mode;s32 available;
 gUnk_0200003C=manager;mode=gUnk_03005430.mode;
 if((u8)(mode-4)<=1||mode==0)goto pause;
 if(gUnk_030053F4&2048)available=stateMode();
 else{
  struct Actor *actor;
  if(gUnk_02000478>0)goto pause;
  actor=gUnk_02000580;if(actor){u32 *address=&actor->flags;u32 mask=16;if(*address&mask)goto pause;}
  available=globalMode();
 }
 asm("" : : "r"(available));
 if(available){pause:available=1;}
 manager->paused=available;
 manager->graphics=sub_08215184(0x1c1a);manager->animationGraphics=sub_08215184(0x1c1c);
 manager->hitResource=sub_0821A520(0x922e,0xd1b8);manager->idleResource=sub_0821A520(0x922e,0xd41f);
 return 0;
}
