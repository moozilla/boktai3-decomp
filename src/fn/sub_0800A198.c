#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {s16 x,y,z,pad;};
struct PackedVec {s32 x:16,y:16,z:16,pad:16;};
struct Sprite {u32 flags;u8 pad4[4];u16 tile;u8 width,height;u8 rest[12];struct Vec position;u8 tail[8];};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Slot {u8 pad0[3];u8 type,kind,active,state,entered,done;u8 pad9;u16 timer,delay,attack,range,script;u8 pad14[84];struct Sprite sprite;struct Anim anim;};
struct Graphics {u32 pad;u16 base;};
struct Manager {u8 pad[28];struct Graphics *graphics;struct Graphics *animationGraphics;void *hitResource;void *animationResource;};
void sub_08217EC4(void *,s32,s32);void sub_08217EEC(void *,void *,u32);void sub_08220C8C(void *,void *,u32,u32,u32);void sub_0822B3BC(u32);
void sub_0800AC44(u32,u32,u32,struct Vec *,struct PackedVec *,u32,u32);
#define CollisionController_UpdateHitAnimation sub_0800A198
void CollisionController_UpdateHitAnimation(struct Manager *manager,struct Slot *p){
 struct Sprite *o;struct Anim *a;const u16 *entry;register u16 n asm("r0");u8 done;struct PackedVec effect;
 if(p->entered){p->entered=0;p->sprite.flags&=~1;sub_08217EC4(&p->sprite,-8,-8);sub_08217EEC(&p->sprite,manager->graphics,0);sub_08220C8C(&p->anim,manager->hitResource,0,0,0);sub_0822B3BC(290);}
 if(p->anim.frame==1&&p->anim.timer==0)p->sprite.position.y+=16;
 if(p->anim.frame==2&&p->anim.timer==0)p->sprite.position.y-=16;
 if(p->anim.frame==4&&p->anim.timer==0){
  effect.x=10;effect.y=10;effect.z=10;
  sub_0800AC44(p->kind,p->type|16,p->attack,&p->sprite.position,&effect,p->range,p->script);
  if(p->type==2)sub_0822B3BC(696);else sub_0822B3BC(285);
 }
 o=&p->sprite;{register struct Graphics *graphics asm("r3")=manager->graphics;asm("" : : "r"(graphics));a=&p->anim;entry=a->data+a->frame;
 {register u32 factor asm("r2")=*entry>>6;asm("" : : "r"(factor));o->tile=(o->width>>3)*(o->height>>3)*factor+graphics->base;}}
 {u32 input=a->flags;register u32 mask asm("r3")=1; register u32 left asm("r1")=(u8)(input&mask); if(left!=(((*entry&0x30)>>4)&mask))o->flags|=4;else o->flags&=~4;}
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
 p->done=done;if(done){p->state=1;p->sprite.flags|=1;}
}
