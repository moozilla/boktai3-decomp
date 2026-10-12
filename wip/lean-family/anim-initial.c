#include "global.h"
struct Obj {u32 flags;u8 pad[12];u16 frame;u8 pad2[12];u16 angle;};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct S {u8 p0[0xddc];struct Obj obj;u8 p1[0xe24 - 0xdfc];struct Anim anim;u8 p2[0xe74 - 0xe34];u32 arg;u16 count;s8 delta;u8 p3[0xf58 - 0xe7b];u8 done,state;u8 p4[0xf64 - 0xf5a];u8 flags;u8 p5[7];u8 last;u8 p6[0xf90 - 0xf6d];void (*fn)(void *);};
void sub_08220C8C(void *,u32,u32,u32,u32);
void sub_081E4188(void *);void sub_08013440(void *);void sub_081E41EC(void *);
static inline u8 take(struct S *p) {
 u32 m=1; u32 v=p->flags;
 if(v&m) {u32 z=0;p->flags=v & ~1;p->done=z;return 1;}return 0;
}
void sub_081E8374(struct S *p) {
 struct Obj *o;struct Anim *a;const u16 *entry;u32 flag;u16 n;u8 done;
 if(take(p)) {
 p->state=2;sub_08220C8C(&p->anim,p->arg,2,0,p->last);
 sub_081E4188(p);sub_08013440(p->p2);sub_081E41EC(p);
 }
 if(p->fn)p->fn(p);
 o=&p->obj;
 if(!(p->count++ &3)) {
 switch((int)(p->anim.frame&3)) {
 case 0:case 3:if(o->flags&0x10) p->delta=1;else p->delta=-8;break;
 case 1:case 2:if(o->flags&0x10) p->delta=-1;else p->delta=8;break;
 }
 o->angle+=p->delta;
 }
 o=&p->obj;a=&p->anim;entry=a->data+a->frame;
 o->frame=*entry>>6;
 if((a->flags&1)!=(((*entry&0x30)>>4)&1))o->flags|=4;else o->flags&=~4;
 if((u8)(a->flags&2)!=(((*entry&0x30)>>4)&2))o->flags|=8;else o->flags&=~8;
 n=++a->timer;
 if(n>=a->rate) {
 a->timer=0;
 if(a->flags&4) {
 if(a->frame==0) {
 if(a->flags&8){a->timer=a->rate;done=1;}else {a->frame=a->count-1;done=1;}
 }else {a->frame--;done=0;}
 }else {
 if(++a->frame>=a->count) {
 if((u8)(a->flags&8)) {a->timer=a->rate;a->frame=a->count-1;done=1;}else {a->frame=0;done=1;}
 }else done=0;
 }
 entry=a->data+a->frame;a->factor=*entry&15;
 a->rate=(a->factor*a->scale)>>6;if(!a->rate)a->rate=1;
 }else done=0;
 p->done=done;
}
