#include "global.h"
struct Vec {s32 x:16;s32 y:16;s32 z:16;s32 pad:16;};
struct Graphics {u32 pad;u16 base;};
struct Sprite {u32 flags;u8 pad4[4];u16 tile;u8 width,height;u8 padC[3];u8 mode,priority;u8 pad11[7];struct Vec position;u8 pad20[8];};
struct Trigger {s16 id,kind;s8 actor;u8 type,flags,flip,persistent,started,visible,flash,tick,cooldown,armed,shake;s16 baseX,baseZ;u8 option,condition;u16 sound;u32 timer;struct Sprite *sprite;void *object,*other,*animation;struct Vec *position;u32 startScript,script,arguments[3];void (*update)(void *,struct Trigger *);u8 effect[108];struct Trigger *previous,*next;};
struct Manager {u8 task[24];struct Graphics *graphics;void *resource;};
struct Manager *sub_08008270(void);
void *sub_08219C40(u32);void sub_08219DD8(void *,u32);void sub_08219D38(void *);
void sub_08007444(void *,struct Trigger *);void sub_08217DE0(struct Sprite *,struct Graphics *,u32);void sub_08217EC4(struct Sprite *,s32,s32);s32 sub_0821D6F4(struct Vec *,s32);s32 sub_0822C034(struct Vec *,s32);void sub_08013930(void *,struct Vec *,s32,s32,s32,s32,s32,s32,s32,s32,s32);void sub_08006FAC(struct Trigger *);
#define Trigger_CreateDrop sub_0800828C
s32 Trigger_CreateDrop(struct Vec *position,s32 id,s32 kind,u32 script,u32 *arguments,s32 sound,s32 persistent,s32 option){
 struct Manager *manager=sub_08008270();struct Trigger *p;s32 i;u32 *in,*out;
 if(!manager)return -1;
 p=sub_08219C40(188);if(!p)return -1;
 sub_08219DD8(p,188);
 p->id=id;p->kind=kind;p->actor=-1;p->type=0;p->flags=0;p->flip=0;
 p->timer=0;p->sprite=0;p->object=0;p->other=0;p->animation=0;p->update=sub_08007444;p->tick=0;p->option=option;p->script=script;
 in=arguments;out=p->arguments;i=2;do{*out++=*in++;}while(--i>=0);
 p->sprite=sub_08219C40(40);if(!p->sprite){sub_08219D38(p);return -1;}
 sub_08219DD8(p->sprite,40);sub_08217DE0(p->sprite,manager->graphics,0);sub_08217EC4(p->sprite,-8,-10);
 if(sub_0821D6F4(position,128)){
 {struct Sprite *sprite=p->sprite;struct Graphics *graphics=manager->graphics;sprite->tile=((sprite->width>>3)*(sprite->height>>3))*35+graphics->base;}p->sprite->mode=1;
 }else{
 {struct Sprite *sprite=p->sprite;struct Graphics *graphics=manager->graphics;sprite->tile=((sprite->width>>3)*(sprite->height>>3))*32+graphics->base;}p->sprite->mode=2;
 }
 p->sprite->position=*position;p->sprite->priority=16;p->position=&p->sprite->position;
 if(sound)p->sound=sub_0822C034(p->position,sound);else p->sound=sound;
 p->persistent=persistent;
 sub_08013930(p->effect,&p->sprite->position,64,24,40,32,512,1,5,1,0);
 sub_08006FAC(p);return 0;
}
