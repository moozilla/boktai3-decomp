#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Obj {u32 flags;u8 pad[12];u16 frame;};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Position {s16 x,y,z;};
struct Trigger {s16 id,kind;s8 actor;u8 type,flags,flip;u8 pad8[16];u32 timer;u8 pad1C[4];struct Obj *obj;u8 pad24[4];struct Anim *anim;struct Position *position;u8 pad30[4];u16 script;u8 pad36[14];void (*update)(void *,struct Trigger *);};
static inline u32 frame_factor(u16 v){register u32 value asm("r1")=v;u32 mask=15;return mask&value;}
struct Manager {u8 pad[28];void *resource;};extern struct Manager *gUnk_02000030;
void sub_08007D84(void *,struct Trigger *);void sub_08220C8C(struct Anim *,void *,u32,u32,u32);
#define Trigger_UpdateLockedPause sub_08007C48
void Trigger_UpdateLockedPause(void *manager,struct Trigger *p){
struct Obj *o;struct Anim *a;const u16 *entry;register u16 n asm("r0");
 o=p->obj;a=p->anim;entry=a->data+a->frame;
 o->frame=*entry>>6;
 {u32 input=a->flags;register u32 mask asm("r3")=1; register u32 left asm("r1")=(u8)(input&mask); if(left!=(((*entry&0x30)>>4)&mask))o->flags|=4;else o->flags&=~4;}
 {register u32 input asm("r0")=a->flags;u32 mask=2; u32 left=(u8)(mask&input); if(left!=(((*entry&0x30)>>4)&mask))o->flags|=8;else o->flags&=~8;}
 n=++a->timer;
 if(n>=a->rate) {
 a->timer=0;
 if(a->flags&4) {
 if(a->frame==0) {
 if(a->flags&8){a->timer=a->rate;}else {a->frame=a->count-1;}
 }else {a->frame--;}
 }else {
 if(++a->frame>=a->count) {
 if((u8)(a->flags&8)) {a->timer=a->rate;a->frame=a->count-1;}else {a->frame=0;}
 }
 }
 entry=a->data+a->frame;a->factor=frame_factor(*entry);
 a->rate=(a->factor*a->scale)>>6;if(!a->rate)a->rate=1;
 }
 if(p->flip)p->obj->flags|=4;
 if(p->timer>31){p->update=sub_08007D84;p->timer=0;sub_08220C8C(p->anim,gUnk_02000030->resource,5,0,4);}
 p->timer++;
}
