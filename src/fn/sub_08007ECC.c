#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {s16 x,y,z,pad;};
struct Graphics {u32 pad;u16 base;};
struct Sprite {u32 flags;u8 pad4[4];u16 tile;u8 width,height;u8 padC[3];u8 mode,priority;u8 pad11[7];struct Vec position;u8 pad20[8];};
struct Object {u32 flags;u8 pad4[24];struct Vec position;u8 pad24[8];};
struct Trigger {s16 id,kind;s8 actor;u8 type,flags,flip,persistent,started,visible,flash,tick,cooldown,armed,shake;s16 baseX,baseZ;u8 option,condition;u16 sound;u32 timer;struct Sprite *sprite;struct Object *object;void *other,*animation;struct Vec *position;u32 startScript,script,arguments[3];void (*update)(void *,struct Trigger *);u8 effect[108];struct Trigger *previous,*next;};
struct Manager {u8 task[24];struct Graphics *graphics;void *resource;u16 kind;u8 pad22[2];u8 soundDelay;u8 pad25[3];struct Trigger *head;struct State *actors[4];struct Vec projected[4];};
struct Manager *sub_08008270(void);
void *sub_08219C40(u32);void sub_08219DD8(void *,u32);void sub_08219D38(void *);
void sub_08007444(void *,struct Trigger *);void sub_08217DE0(struct Sprite *,struct Graphics *,u32);void sub_08217EC4(struct Sprite *,s32,s32);s32 sub_0821D6F4(struct Vec *,s32);s32 sub_0822C034(struct Vec *);void sub_08013930(void *,struct Vec *,s32,s32,s32,s32,s32,s32,s32,s32,s32);void sub_08006FAC(struct Trigger *);
struct Map {u32 pad0;u8 *cells;u8 pad8[28];u16 rows[1];};
extern struct Map *gUnk_030052F4;extern u32 gUnk_030052F8,gUnk_030052FC;
u8 *sub_0821D64C(s32,s32);
static inline s32 cellIndex(struct Vec *pos){s32 x=pos->x>>8,z=pos->z>>8;if(x<0||z<0||(u32)x>=gUnk_030052F8||(u32)z>=gUnk_030052FC)return 0;return gUnk_030052F4->rows[z]+x;}
static inline s32 groundHeight(struct Vec *position){
 s32 index=cellIndex(position),height;u8 *cell=sub_0821D64C(index,1);u32 kind;
 if(cell)cell+=4;else cell=gUnk_030052F4->cells+12+index*4;
 kind=*cell>>4;height=(*cell&15)<<8;
 switch(kind){case 1:height-=(u8)position->z;break;case 2:height-=(u8)position->x;break;}
 return height;
}
struct State {u8 pad0[48];struct Vec position;u8 pad38[0x410];u32 flags;u8 pad44C[0xE8];u16 mode,pad536,active;};
extern struct State *gUnk_02000580[4];extern u32 gUnk_030053F4;extern s32 gUnk_02000478;extern u16 gUnk_02000534,gUnk_02000500;
static inline s32 stateMode(void){struct State *state=gUnk_02000580[0];if(state){u32 active=state->active;s32 yes=0;if(active&&state->mode==4)yes=1;return yes;}return 0;}
static inline s32 globalMode(void){u32 active=gUnk_02000534;s32 yes=0;if(active&&gUnk_02000500==4)yes=1;return yes;}
static inline s32 actorMode(void){struct State *state=gUnk_02000580[0];if(state){u32 mask=16;return state->flags&mask;}return 0;}
void sub_080075F8(void *,struct Trigger *);void sub_080077FC(void *,struct Trigger *);void sub_0822B2F8(s32);s32 sub_082151E4(void *,u16);void sub_082144A4(struct Object *,void *,s32);void sub_08215284(void *,s32);void sub_08220C8C(void *,void *,s32,s32,s32);void sub_08013728(void *);
static inline s32 scale(s32 input){s32 value=input*3;if(value>=0)value>>=5;else value=-((-value)>>5);return value;}
static inline s32 scaleHeight(s32 input){s32 value=input*3;if(value>=0)return value>>5;return -((-value)>>5);}
struct World {u8 pad[0x5A0];u32 region;};extern struct World *gUnk_02000710;
void sub_0800700C(struct Trigger *);void sub_08249240(struct Manager *,struct Trigger *,void (*)(void *,struct Trigger *));
static inline u8 sameRegion(struct Trigger *p){u32 current=({struct World *world=gUnk_02000710;register u32 offset asm("r2")=0x5A0;asm("" : : "r"(offset));*(u32 *)((u8 *)world+offset);});register u32 region asm("r7")=p->sound;if(current==region)return 1;return 0;}
#define TriggerManager_Update sub_08007ECC
s32 TriggerManager_Update(struct Manager *input){struct Manager *p=input;s32 i;struct Trigger *trigger,*next;register u8 *delay asm("r9");
 {struct State **table;i=0;table=gUnk_02000580;
 for(;i<4;i++){
  struct State *actor;asm("" : "+r"(i));actor=({struct State *a=table[i];a;});p->actors[i]=actor;
  if(actor){register u32 offset asm("r0")=i*8;struct Vec *out;struct Vec *in;s32 x,z,horizontal,height;out=(struct Vec *)(offset+(u32)({register struct Manager *manager asm("r7")=p;manager;}));out=(struct Vec *)((u8 *)out+60);in=&actor->position;x=({register s32 value asm("r4")=actor->position.x;asm("" : : "r"(value));value;});z=in->z;
   out->x=scale(x-z);horizontal=scale(({register s32 sum asm("r1")=x+z;asm("" : : "r"(sum));sum;}));height=scaleHeight(in->y);out->y=horizontal-height;out->z=horizontal+height;
  }
 }
 }
 trigger=({register struct Manager *m asm("r0")=p;m->head;});
 delay=({register u8 *value asm("r1")=(u8 *)36;value=(u8 *)((u32)value+(u32)p);value;});
 for(;trigger;trigger=next){
  next=trigger->next;
  if(trigger->flags&1)sub_0800700C(trigger);
  else{
   if(!trigger->sound||sameRegion(trigger)){
    if(trigger->sprite)trigger->sprite->flags&=~1;
    {struct Object *object;struct Object *candidate=({struct Object *value=trigger->object;value;});object=candidate;
    if(candidate){
     if(!trigger->started)goto showObject;
     else if(gUnk_02000478>0){candidate->flags&=~1;trigger->flash=0;trigger->tick=0;trigger->visible=1;}
     else{
      s32 available;
      if(gUnk_030053F4&2048)available=stateMode();
      else if(gUnk_02000580[0]&&({u32 *flags=&gUnk_02000580[0]->flags;u32 mask=16;*flags&mask;}))goto available;
      else available=globalMode();
      asm("" : : "r"(available));
      if(available){
available:
       if(trigger->visible)trigger->flash=31;
       {u32 ticks=trigger->flash;register u32 saved asm("r2")=ticks;if(saved){ticks--;trigger->flash=ticks;ticks&=15;if(ticks>7)object->flags&=~1;else object->flags|=1;}
       else{if(trigger->tick<=47)object->flags&=~1;else if(trigger->tick<=71)object->flags|=1;else trigger->tick=saved;trigger->tick++;}}
       trigger->visible=0;
      }else{
       if(!trigger->visible)trigger->flash=47;
       {u32 ticks=trigger->flash;if(ticks){ticks--;trigger->flash=ticks;if((ticks&15)>7)object->flags|=1;else object->flags&=~1;trigger->tick=0;}
       else object->flags|=1;}
       trigger->visible=1;
      }
     }
    }
     goto updatedVisibility;
showObject:candidate->flags&=~1;
updatedVisibility:;
   }}else{
    if(trigger->sprite)trigger->sprite->flags|=1;
    if(trigger->object)trigger->object->flags|=1;
   }
   sub_08249240(p,trigger,trigger->update);
  }
 }
 {u8 *timer=({register u8 *value asm("r2")=delay;asm("" : : "r"(value));value;});u32 ticks=*timer;if(ticks)*timer=ticks-1;}
 return 0;
}
