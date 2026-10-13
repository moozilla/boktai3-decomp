#include "global.h"
typedef struct ActorAnimation {u8 pad0[0x51];u8 angle;u8 pad52[0x4a];u32 flags;u8 padA0[8];u8 facing,facing8;u8 padAA[5];u8 finished,enter;u8 changed;u8 padB2;u8 locked;u8 padB4[0x18];u8 requestedAngle;u8 padCD[0x37];u8(*tryUpdate)(void*);u8(*reset)(void*);u8 pad10C[8];s8 savedAngle;u8 pad115[0x22e];u8 variant;s16 scale,minScale,maxScale,scaleStep;u8 pad34C[0x60];u32 mode;} ActorAnimation;

static inline void setFacing(ActorAnimation*p,int angle){u32 value;int shifted;p->angle=angle;shifted=p->angle+0x60;shifted>>=6;value=3&shifted;p->facing=value;shifted=p->angle+0x70;shifted>>=5;value=7&shifted;p->facing8=value;}
void sub_0802175C(void*,void*,void*,int,int,int,int);void sub_0822B3BC(int);int sub_082215E4(int,int);extern s16 gUnk_086149C4[];
static inline int choose(u32 mode,int normal,int alternate){int test=mode&1;int result;result=normal;if(test)result=alternate;return result;}
static inline int trunc12(int x){return x>=0?x>>12:-(-x>>12);}
void sub_08025714(ActorAnimation*p,int tick){u8*position=(u8*)p+0x48;if(p->changed)p->changed=0;{u32 mask=0x40000;p->flags|=mask;}if(!p->tryUpdate(p)){{u32 mask=64;p->flags|=mask;}if(!p->reset(p)){if(tick<=10){int rate=10-tick;s16*velocity=(s16*)((u8*)p+0x5c);u32 angle=position[9];int speed;speed=rate*4;{int x=trunc12(gUnk_086149C4[(angle+64)&255]*speed);int zero=0;velocity[0]=x;velocity[1]=zero;}velocity[2]=trunc12(gUnk_086149C4[angle]*speed);}if(tick>9)p->finished=1;}}}
