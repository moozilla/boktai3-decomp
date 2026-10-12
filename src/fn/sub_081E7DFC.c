#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */
struct Obj {u32 flags;u8 pad[12];u16 frame;u8 pad2[12];u16 angle;};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct S {u8 p0[0xddc];struct Obj obj;u8 p1[0xe24 - 0xdfc];struct Anim anim;u8 p2[0xe74 - 0xe34];u32 arg;u16 count;s8 delta;u8 p3[0xf58 - 0xe7b];u8 done,state;u8 p4[0xf64 - 0xf5a];u8 flags;u8 p5[7];u8 last;u8 p6[0xf8c - 0xf6d];void (*previous)(void *);void (*fn)(void *);};
void sub_08220C8C(void *,u32,u32,u32,u32);
void sub_081E4188(void *);void sub_08013440(void *);void sub_081E41EC(void *);
static inline u8 take(struct S *p) {s32 m=1;register s32 v asm("r1")=p->flags;if(v&m) {p->flags=~m&v;p->done=0;return 1;}return 0;}
static inline u32 frame_factor(u16 v) {register u32 value asm("r1")=v;u32 mask=15;return mask&value;}
void sub_08215284(void*,s32);void sub_08013424(void*);void sub_081E38C4(void*);
void sub_081E7DFC(struct S *p) {
 struct Obj *o;struct Anim *a;const u16 *entry;u32 flag;register u16 n asm("r0");u8 done;
 if(take(p)) {
 p->state=12;sub_08220C8C(&p->anim,p->arg,12,0,0);
 sub_08215284((u8*)p+0xe08,346);
 o=&p->obj;a=&p->anim;entry=a->data+a->frame;
 o->frame=*entry>>6;
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
 entry=a->data+a->frame;a->factor=frame_factor(*entry);
 a->rate=(a->factor*a->scale)>>6;if(!a->rate)a->rate=1;
 }else done=0;
 *((u8*)p+0xde3)=3;p->obj.flags&=~1;
 }
}
