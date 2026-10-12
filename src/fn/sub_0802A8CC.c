// MATCHING_CONCESSION: register-pinning
#include "global.h"
typedef struct ActorAnimation {u8 pad0[0x51];u8 angle;u8 pad52[0x4a];u32 flags;u8 padA0[8];u8 facing,facing8;u8 padAA[5];u8 finished,enter;u8 changed;u8 padB2;u8 locked;u8 padB4[0x18];u8 requestedAngle;u8 padCD[0x37];u8(*tryUpdate)(void*);u8(*reset)(void*);u8 pad10C[8];s8 savedAngle;u8 pad115[0x6a];u8 stage;u32 counter;u8 pad184[0x1bf];u8 variant;s16 scale,minScale,maxScale,scaleStep;u8 pad34C[0x60];u32 mode;} ActorAnimation;

static inline void setFacing(ActorAnimation*p,int angle){register u32 value asm("r1");register int shifted asm("r0");p->angle=angle;shifted=p->angle+0x60;asm("" : "+r"(shifted));shifted>>=6;asm("" : "+r"(shifted));value=3&shifted;asm("" : "+r"(value));p->facing=value;shifted=p->angle+0x70;asm("" : "+r"(shifted));shifted>>=5;asm("" : "+r"(shifted));value=7&shifted;asm("" : "+r"(value));p->facing8=value;}
void sub_080297FC(void*,int,int,int);void sub_08029E1C(void*);int sub_08029E2C(void*);void sub_08029AA8(void*);void sub_0822B3BC(int);
void sub_0802A8CC(ActorAnimation*p,int tick){if(p->changed){p->changed=0;sub_08029E1C(p);sub_080297FC(p,sub_08029E2C(p),1,8);}*(u32*)((u8*)p+0x178)=0;*((u8*)p+0x17c)=0;{u32 mask=12;p->flags|=mask;}if(!p->reset(p)&&p->padB2){sub_080297FC(p,sub_08029E2C(p),1,8);if(tick>31)p->finished=1;}}
