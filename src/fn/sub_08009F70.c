#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Sprite {u32 flags;u8 pad4[4];u16 tile;u8 width,height;u8 rest[28];};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Slot {u8 pad0[7];u8 entered,done;u8 pad9;u16 timer,delay;u8 padE[90];struct Sprite sprite;struct Anim anim;};
struct Graphics {u32 pad;u16 base;};
struct Manager {u8 pad[28];void *graphics;struct Graphics *animationGraphics;u32 pad24;void *animationResource;};
extern u32 gUnk_03005308;extern u16 gUnk_0203B400[];
void sub_08217EC4(void *,s32,s32);void sub_08217EEC(void *,void *,u32);void sub_08220C8C(void *,void *,u32,u32,u32);
static inline u32 randomDelay(u32 restart){gUnk_03005308=(gUnk_03005308+1)&1023;{register u32 offset asm("r0")=gUnk_03005308*2;register u32 value asm("r0");asm("" : : "r"(offset));if(restart){register u16 *table asm("r2")=gUnk_0203B400;asm("" : : "r"(table));value=*(u16 *)(offset+(u32)table);}else value=*(u16 *)((u8 *)gUnk_0203B400+offset);asm("" : : "r"(value));value&=127;value+=120;asm("" : : "r"(value));return value;}}
#define CollisionController_UpdateIdleAnimation sub_08009F70
void CollisionController_UpdateIdleAnimation(struct Manager *manager,struct Slot *p){
 struct Sprite *o;struct Anim *a;const u16 *entry;register u16 n asm("r0");u8 done;
 if(p->entered){p->entered=0;sub_08217EC4(&p->sprite,-4,-4);sub_08217EEC(&p->sprite,manager->animationGraphics,0);sub_08220C8C(&p->anim,manager->animationResource,8,0,0);p->sprite.flags&=~1;p->delay=randomDelay(0);}
 {register u32 flags asm("r0")=p->sprite.flags;register u32 low asm("r2")=1;register u32 firstMask asm("ip");asm("" : : "r"(flags),"r"(low));firstMask=low;
 if(flags&low){
  if(p->timer++>p->delay){p->timer=0;p->delay=randomDelay(1);sub_08217EC4(&p->sprite,-4,-4);sub_08217EEC(&p->sprite,manager->animationGraphics,0);sub_08220C8C(&p->anim,manager->animationResource,8,0,0);p->sprite.flags&=~1;}
 }else{
 o=&p->sprite;{register struct Graphics *graphics asm("r3")=manager->animationGraphics;asm("" : : "r"(graphics));a=&p->anim;entry=a->data+a->frame;
 {register u32 factor asm("r2")=*entry>>6;asm("" : : "r"(factor));o->tile=(o->width>>3)*(o->height>>3)*factor+graphics->base;}}
 {register u32 input asm("r0")=a->flags;register u32 left asm("r1")=firstMask;left&=input;asm("" : "+r"(firstMask)); if(left!=(((*entry&0x30)>>4)&firstMask))o->flags|=4;else o->flags&=~4;}
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
 entry=a->data+a->frame;a->rate=((*entry&15)*a->scale)>>6;
 }else done=0;
 p->done=done;if(done)p->sprite.flags|=1;
 }
}}
