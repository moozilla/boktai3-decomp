// MATCHING_CONCESSION: register-pinning
#include "global.h"
typedef struct ActorAnimation {u8 pad0[0x51];u8 angle;u8 pad52[0x4a];u32 flags;u8 padA0[8];u8 facing,facing8;u8 padAA[5];u8 finished,enter;u8 changed;u8 padB2;u8 locked;u8 padB4[0x18];u8 requestedAngle;u8 padCD[0x37];u8(*tryUpdate)(void*);u8(*reset)(void*);u8 pad10C[8];s8 savedAngle;u8 pad115[0x22e];u8 variant;s16 scale,minScale,maxScale,scaleStep;u8 pad34C[0x60];u32 mode;} ActorAnimation;

static inline void setFacing(ActorAnimation*p,int angle){register u32 value asm("r1");register int shifted asm("r0");p->angle=angle;shifted=p->angle+0x60;asm("" : "+r"(shifted));shifted>>=6;asm("" : "+r"(shifted));value=3&shifted;asm("" : "+r"(value));p->facing=value;shifted=p->angle+0x70;asm("" : "+r"(shifted));shifted>>=5;asm("" : "+r"(shifted));value=7&shifted;asm("" : "+r"(value));p->facing8=value;}
void sub_0802175C(void*,void*,void*,int,int,int,int);void sub_0822B3BC(int);int sub_082215E4(int,int);extern s16 gUnk_086149C4[];
static inline int choose(u32 mode,int normal,int alternate){register int test asm("r0")=mode&1;int result;asm("":"+r"(test));result=normal;if(test)result=alternate;return result;}
static inline int trunc12(int x){return x>=0?x>>12:-(-x>>12);}
void sub_080216E8(void*,void*,void*,int,int,int);
typedef struct {u32 a,b;} Point;
void sub_08021890(void*,void*,void*,int,int,int);u8 sub_082212DC(int*,int,int,int);
void sub_080282BC(ActorAnimation*p,int tick){ActorAnimation*p2=p;u8*position=(u8*)p+0x48;int done;if(p->changed){register int sequence asm("r5");register int one asm("r3");p->changed=0;{register int test asm("r0")=p->mode;one=1;asm("":"+r"(one):"r"(test));test&=one;asm("":"+r"(test));sequence=70;if(test)sequence=32;}asm("":"+r"(sequence));{register int offset asm("r0")=0x240;asm(""::"r"(offset));sub_08021890(p,(u8*)p+offset,(u8*)p+0x220,sequence,one,4);}*(Point*)((u8*)p+0xd0)=*(Point*)(position+12);}done=0;if(sub_082212DC(&done,tick,10,30)){u32 mask=16;p->flags|=mask;}else if(done){u32 mask=16;p->flags|=mask;}*(u32*)((u8*)p2+0x33c)=0;*((u8*)p2+0x340)=0;{u32 mask=12;p->flags|=mask;}if(!p->reset(p)&&tick>59)p->finished=1;}
