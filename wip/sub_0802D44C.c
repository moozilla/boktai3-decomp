// WIP: nonmatching candidate; never merge into production src or count as exact.
#include "global.h"
struct Vec{s16 x,y,z,pad;};struct Object{u8 pad0[0xc];struct Vec pos;u8 pad14[0x3c];u32 flags;u8 pad54[0xc8];void*actionFn;void*selectFn;void*damageFn;u16 state,action;u8 stateChanged,actionChanged;u16 requested;u32 ticks,actionTicks;u8 pad138[0x4c];u32 drawable;};
void sub_0802CD04(struct Object*);void sub_08042588(void*,int,int,int,struct Vec*);int sub_0802D080(struct Object*,int,int,int,int,int);void sub_0802C9A0(struct Object*,struct Vec*,struct Vec*,void*);void sub_0802CA1C(struct Object*,struct Vec*,struct Vec*,void*);void sub_0802D1C8(void);void sub_0802D1B0(void);void sub_0802D334(void);void sub_0802D380(void);void sub_0802D218(void);
static inline void selectCallback(struct Object*p,void*fn){p->selectFn=fn;}
static inline void damageCallback(struct Object*p,void*fn){p->damageFn=fn;}
static inline void state(struct Object*p,int zero){p->state=zero;p->stateChanged=1;p->ticks=zero;}
static inline void action(struct Object*p,void*fn,int zero,int one){p->actionFn=fn;p->action=zero;p->actionChanged=one;p->actionTicks=zero;}
static inline void collider(struct Object*p,struct Vec*size,void*fn,struct Vec*offset){sub_0802C9A0(p,size,offset,fn);}
static inline void hitbox(struct Object*p,struct Vec*size,void*fn,struct Vec*offset){sub_0802CA1C(p,size,offset,fn);}
int sub_0802D44C(struct Object*p){struct Vec size,offset,size2,offset2;int zero;sub_0802CD04(p);sub_08042588(&p->drawable,0xd495,0xaf44,0,&p->pos);{register int initialZero asm("r2")=0;asm("" : "+r"(initialZero));zero=initialZero;sub_0802D080(p,1,initialZero,1,initialZero,initialZero);}{u32 mask=2;p->flags|=mask;}asm("" : "+r"(zero) : : "r9");size.x=32;size.y=128;size.z=32;offset.x=0;offset.y=128;offset.z=0;collider(p,&size,sub_0802D1C8,&offset);size2.x=32;size2.y=128;size2.z=32;offset2.x=0;offset2.y=128;offset2.z=0;hitbox(p,&size2,sub_0802D1B0,&offset2);selectCallback(p,sub_0802D334);damageCallback(p,sub_0802D380);p->state=zero;p->stateChanged=1;p->ticks=zero;action(p,sub_0802D218,zero,1);return 0;}
