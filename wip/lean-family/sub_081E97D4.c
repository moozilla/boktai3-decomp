#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Tentative role: initialize and advance a rotating animated effect.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */
struct Obj {u32 flags;u8 pad[12];u16 frame;u8 pad2[12];u16 angle;};
struct Anim {const u16 *data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct S {u8 p0[0xddc];struct Obj obj;u8 p1[0xe24 - 0xdfc];struct Anim anim;u8 p2[0xe74 - 0xe34];u32 arg;u16 count;s8 delta;u8 p3[0xf58 - 0xe7b];u8 done,state;u8 p4[0xf64 - 0xf5a];u8 flags;u8 p5[7];u8 last;u8 p6[0xf90 - 0xf6d];void (*fn)(void *);};
void sub_08220C8C(void *,u32,u32,u32,u32);
void sub_081E4188(void *);void sub_08013440(void *);void sub_081E41EC(void *);
static inline u8 take(struct S *p) {s32 m=1;register s32 v asm("r1")=p->flags;if(v&m) {p->flags=~m&v;p->done=0;return 1;}return 0;}
struct Rotator {u32 flags;u8 pad[12];u16 frame;u8 pad2[12];u16 angle;u8 pad20[0x7e];s8 delta;};
static inline void rotate(struct S *p) {
 struct Rotator *o=(struct Rotator *)&p->obj;
 if(!(p->count++ &3)) {
 switch((int)(p->anim.frame&3)) {
 case 0:case 3:if(o->flags&0x10) o->delta=1;else o->delta=-8;break;
 case 1:case 2:if(o->flags&0x10) o->delta=-1;else o->delta=8;break;
 }
 o->angle+=o->delta;
 }
}
void sub_081E97D4(struct S *p) {
 s32 x,y;
 if(take(p)) sub_08013440(p->p2);
 rotate(p);
 x=*(s8 *)((u8 *)&p->obj+8);
 y=*(s8 *)((u8 *)&p->obj+9);
 x++;y++;
 if(x>127) {y=127;x=127;}
 *((u8 *)&p->obj+8)=x;
 *((u8 *)&p->obj+9)=y;
}
