// MATCHING_CONCESSION: register-pinning
#include "global.h"
typedef struct ActorAnimation {u8 pad0[0x51];u8 angle;u8 pad52[0x4a];u32 flags;u8 padA0[8];u8 facing,facing8;u8 padAA[5];u8 finished,enter;u8 changed;u8 padB2;u8 locked;u8 padB4[0x18];u8 requestedAngle;u8 padCD[0x37];u8(*tryUpdate)(void*);u8(*reset)(void*);u8 pad10C[8];s8 savedAngle;u8 pad115[0x22e];u8 variant;s16 scale,minScale,maxScale,scaleStep;u8 pad34C[0x60];u32 mode;} ActorAnimation;

static inline void setFacing(ActorAnimation*p,int angle){u32 value;register int shifted asm("r0");p->angle=angle;shifted=p->angle+0x60;asm("" : "+r"(shifted));shifted>>=6;value=3&shifted;p->facing=value;shifted=p->angle+0x70;asm("" : "+r"(shifted));shifted>>=5;value=7&shifted;p->facing8=value;}
void sub_0802175C(void*,void*,void*,int,int,int,int);void sub_0822B3BC(int);int sub_082215E4(int,int);extern s16 gUnk_086149C4[];
static inline int choose(u32 mode,int normal,int alternate){int test=mode&1;int result;result=normal;if(test)result=alternate;return result;}
static inline int trunc12(int x){return x>=0?x>>12:-(-x>>12);}
static inline void decay(s16*p){register s16*dst asm("r1")=p;u32 old=(u16)*dst;register int next asm("r0");next=*dst>23?old-24:0;*dst=next;}
void sub_0802533C(ActorAnimation*p,int tick){ActorAnimation*p2=p;u8*position=(u8*)p+0x48;if(!p->locked)setFacing(p,p->requestedAngle);if(p->changed){int sequence;p->changed=0;if(p->locked){s16*here=(s16*)((u8*)p+0x54);s16*dest;dest=(s16*)((u8*)p+0x9c4);p->savedAngle=sub_082215E4(dest[0]-*(s16*)(position+12),dest[2]-here[2]);setFacing(p,p->savedAngle);}else{if(*(u16*)((u8*)p+0x3b4)==2)sub_0822B3BC(889);else sub_0822B3BC(888);}sequence=choose(p2->mode,51,13);sub_0802175C(p,(u8*)p2+0x240,(u8*)p2+0x220,sequence,2,3,4);}else{int sequence=choose(p->mode,51,13);sub_0802175C(p,(u8*)p+0x240,(u8*)p+0x220,sequence,2,3,1);}
 decay(&p2->minScale);decay(&p2->maxScale);
 if(!p->tryUpdate(p)){u32 mask=64;p->flags|=mask;if(!p->reset(p)){if(tick<=63){register int rate asm("r0")=((int)mask-tick)>>3;register s16*velocity asm("r3")=(s16*)(position+20);u32 angle=position[9];register int speed asm("r6");speed=-rate;{int x=trunc12(gUnk_086149C4[(angle+64)&255]*speed);register int zero asm("r0")=0;velocity[0]=x;velocity[1]=zero;}velocity[2]=trunc12(gUnk_086149C4[angle]*speed);}if(tick>63)p->finished=1;}}}
