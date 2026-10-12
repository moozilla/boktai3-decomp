#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {s16 x,y,z,pad;};
struct Sprite {u32 flags;u8 pad4[12];u16 frame;u8 pad12[10];struct Vec position;u8 tail[8];};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Object {u16 id;u8 type,entered,state,flags;u16 timer,parameter;u8 animationDone,effectActive,animated,remove,frameTimer,frame;s16 health;u16 speed,ground,pad16;struct Vec position;void *collider,*attack;u8 resource[28];struct Sprite sprite;struct Anim anim;u8 effect[108];struct Object *previous,*next;};
struct Manager {u8 task[24];void *animation;struct Object *head;};extern struct Manager *gUnk_0200046C;
void sub_08220C8C(void *,void *,u32,u32,u32);void sub_0822B2F8(u32);void sub_0821FF24(void *,void *,u32);void sub_0821FE40(void *);
void sub_0800CC50(void *);typedef void (*Callback)(struct Object *);extern Callback gUnk_08604ACC[][4];
#define FallingObjectManager_Update sub_0800D668
s32 FallingObjectManager_Update(struct Manager *manager){
struct Object *p=manager->head,*next;struct Sprite *o;struct Anim *a;const u16 *entry;register u16 n asm("r0");u8 done;register u32 one asm("r9");
if(p){register u32 initial asm("r0")=1;asm("" : : "r"(initial));one=initial;}
for(;p;p=next){next=p->next;if(p->remove){sub_0800CC50(p);continue;}
if(p->animated){
o=&p->sprite;a=&p->anim;entry=a->data+a->frame;o->frame=*entry>>6;
 {u32 input=a->flags;register u32 left asm("r2")=(u8)(one&input);asm("" : "+r"(one)); if(left!=(((*entry&0x30)>>4)&one))p->sprite.flags|=4;else p->sprite.flags&=~4;}
 {register u32 input asm("r0")=a->flags;register u32 mask asm("r3")=2; register u32 left asm("r1")=(u8)(mask&input); if(left!=(((*entry&0x30)>>4)&mask))o->flags|=8;else o->flags&=~8;}
 n=++a->timer;
 if(n>=a->rate) {
 a->timer=0;
 if(a->flags&4) {
 if(a->frame==0) {
 if(a->flags&8){a->timer=a->rate;}else {a->frame=a->count-1;}done=1;
 }else {a->frame--;done=0;}
 }else {
 if(++a->frame>=a->count) {
 if((u8)(a->flags&8)) {a->timer=a->rate;a->frame=a->count-1;done=1;}else {a->frame=0;done=1;}
 }else done=0;
 }
 entry=a->data+a->frame;{register u32 frame asm("r1")=*entry;asm("" : : "r"(frame));a->factor=frame&15;}a->rate=(a->factor*a->scale)>>6;if(!a->rate)a->rate=one;
 }else done=0;
p->animationDone=done;
}
p->sprite.position=p->position;gUnk_08604ACC[p->type][p->state](p);
}
return 0;
}
