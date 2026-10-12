#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {s16 x,y,z,pad;};
struct PackedVec {s32 x:16,y:16,z:16,pad:16;};
struct Sprite {u32 flags;u8 pad4[4];u16 tile;u8 width,height;u8 rest[12];struct Vec position;u8 tail[8];};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Particle {u8 state,speed,angle; s8 drift;u16 timer,lifetime;struct Vec velocity,origin;struct Anim anim;struct Sprite sprite;};
struct Effect {u8 active,count,type,kind,soundDelay,pad;u16 timer;struct Particle particles[4];};
struct Graphics {u32 pad;u16 base;};
struct Manager {u8 pad[32];struct Graphics *graphics;};
void sub_0800A80C(struct Manager *);
#define EffectParticle_UpdateFinish sub_0800C0B4
void EffectParticle_UpdateFinish(struct Manager *manager,struct Effect *effect,s32 index){
struct Particle *p=&effect->particles[index];struct Sprite *sprite=&p->sprite,*o;struct Anim *a;const u16 *entry;register u16 n asm("r0");u8 done;
 o=sprite;{register struct Graphics *graphics asm("r3")=manager->graphics;asm("" : : "r"(graphics));a=&p->anim;entry=a->data+a->frame;
 {register u32 factor asm("r2")=*entry>>6;asm("" : : "r"(factor));o->tile=(o->width>>3)*(o->height>>3)*factor+graphics->base;}}
 {u32 input=a->flags;register u32 mask asm("r3")=1; register u32 left asm("r1")=(u8)(input&mask); if(left!=(((*entry&0x30)>>4)&mask))p->sprite.flags|=4;else p->sprite.flags&=~4;}
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
 if(done){p->state=0;sprite->flags|=1;effect->count--;if(!effect->count)sub_0800A80C(manager);}
}
