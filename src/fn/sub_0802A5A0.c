// MATCHING_CONCESSION: register-pinning
#include "global.h"
typedef struct ActorAnimation {u8 pad0[0x51];u8 angle;u8 pad52[0x4a];u32 flags;u8 padA0[8];u8 facing,facing8;u8 padAA[5];u8 finished,enter;u8 changed;u8 padB2;u8 locked;u8 padB4[0x18];u8 requestedAngle;u8 padCD[0x37];u8(*tryUpdate)(void*);u8(*reset)(void*);u8 pad10C[8];s8 savedAngle;u8 pad115[0x6a];u8 stage;u32 counter;u8 pad184[0x1bf];u8 variant;s16 scale,minScale,maxScale,scaleStep;u8 pad34C[0x60];u32 mode;} ActorAnimation;

static inline void setFacing(ActorAnimation*p,int angle){register u32 value asm("r1");register int shifted asm("r0");p->angle=angle;shifted=p->angle+0x60;asm("" : "+r"(shifted));shifted>>=6;asm("" : "+r"(shifted));value=3&shifted;asm("" : "+r"(value));p->facing=value;shifted=p->angle+0x70;asm("" : "+r"(shifted));shifted>>=5;asm("" : "+r"(shifted));value=7&shifted;asm("" : "+r"(value));p->facing8=value;}
void sub_080297FC(void*,int,int,int);void sub_08029E1C(void*);int sub_08029E2C(void*);void sub_08029AA8(void*);void sub_0822B3BC(int);
struct Common {u8 pad0[0x90];u32 timer;s16 health,maxHealth;u8 pad98[4];u32 flags;u8 padA0[2];u8 reset;u8 padA3;u32 pending;u8 facing,facing8,requested,state;u8 padAC[5];u8 changed;u8 padB2[0x4e];void(*callback)(void*);};
static inline void state(struct Common*p,void(*fn)(void*),int id){int zero;p->state=id;zero=0;p->callback=fn;p->timer=zero;p->changed=1;p->reset=zero;p->pending=zero;}void sub_08046834(void*);void sub_08029FF8(void*);
void sub_0802A5A0(ActorAnimation*p){if(p->changed){p->changed=0;sub_080297FC(p,8,1,8);sub_08046834(*(void**)((u8*)p+0x904));sub_0822B3BC(1359);}{u8*dst=(u8*)p+0x197;int zero=0;*dst=6;*(u32*)((u8*)p+0x178)=zero;*((u8*)p+0x17c)=zero;}{u32 mask=12;p->flags|=mask;}if(p->padB2)state((void*)p,sub_08029FF8,1);else p->flags|=1;}
