#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {s16 x,y,z;};
struct Actor {u8 pad0[8];u8 index;u8 pad9[3];struct Vec position;};
struct State {u8 pad[0x418];u8 mode;};
struct Trigger {s16 id,kind;s8 actor;u8 type,flags,flip;u8 persistent,started;u8 padA[11];u8 condition;u8 pad16[2];u32 timer;u8 pad1C[12];void *animation;struct Vec *position;u16 script;u8 pad32[18];void (*update)(void *,struct Trigger *);u8 pad48[112];struct Trigger *next;};
struct Manager {u8 pad0[28];void *resource;u8 pad20[4];u8 soundDelay;u8 pad25[3];struct Trigger *head;};
extern struct Manager *gUnk_02000030;
extern struct State *gUnk_02000580[];
extern u32 gUnk_030053F4;
void sub_0822B2F8(s32);void sub_0822B3BC(s32);void sub_08007908(void *,struct Trigger *);void sub_08220C8C(void *,void *,s32,s32,s32);s32 sub_0822D490(s32,s32);s32 sub_08006F00(struct Trigger *,u16,s32);
static inline s32 absolute(s32 value){if(value<0)value=-value;return value;}
static inline u8 nearby(struct Vec *p,struct Vec *a,struct Actor *actor){
 if(absolute(p->x-actor->position.x)<=95&&absolute(p->y-a->y)<=127){
 s32 distance=absolute(p->z-a->z);register u32 yes asm("r0")=1;asm("" : : "r"(yes));if(distance<=95)return yes;
 }return 0;
}
#define Trigger_TryOpenNearby sub_08007304
s32 Trigger_TryOpenNearby(struct Actor *actor){
 struct Manager *manager=gUnk_02000030;struct Trigger *p;s32 index;
 if(!manager)return 0;
 index=actor->index;
 for(p=manager->head;p;){
  struct State *state;
  if((p->flags&1)||*(u16 *)&p->actor!=511)goto next;
  {register struct Vec *point asm("r3")=&actor->position;asm("" : : "r"(point));if(!nearby(p->position,point,actor))goto next;}
  state=gUnk_02000580[index];
  if(state&&state->mode==6&&p->condition==1){
   if(!manager->soundDelay){sub_0822B2F8(402);manager->soundDelay=15;
next: p=p->next;
}else {p=p->next;}
  }else{
  p->actor=index;sub_0822B3BC(271);p->update=sub_08007908;p->timer=0;p->started=0;
  sub_08220C8C(p->animation,manager->resource,5,0,0);
  {register s32 event asm("r2");
   if(gUnk_030053F4&2048)event=1;
   else event=sub_0822D490(p->id,p->kind)==1;
   sub_08006F00(p,p->script,event);}
  return 1;
  }
 }
 return 0;
}
